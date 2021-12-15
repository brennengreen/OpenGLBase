#include "Teapot.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Input.h"
#include "Application.h"

#include "stb_image.h"

Teapot::Teapot()
{
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, Application::GetWindowExtent().x, Application::GetWindowExtent().y);

	_init_pipelines();
	_init_imgui();

	Input::Update();
}

Teapot::~Teapot() {
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Teapot::_init_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Application::GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Teapot::ProcessKeyboardState()
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Input::SetCursorEnabled(true);
	if (Input::IsKeyPressed(GLFW_KEY_ENTER))
		Input::SetCursorEnabled(false);
	if(Input::IsKeyPressed(GLFW_KEY_W) && !Input::IsCursorEnabled())
		Cam.ProcessKeyboard(CameraMovement::FORWARD, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_A) && !Input::IsCursorEnabled())
		Cam.ProcessKeyboard(CameraMovement::LEFT, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_S) && !Input::IsCursorEnabled())
		Cam.ProcessKeyboard(CameraMovement::BACKWARD, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_D) && !Input::IsCursorEnabled())
		Cam.ProcessKeyboard(CameraMovement::RIGHT, _deltaTime);
}

void Teapot::ProcessScrollState()
{
	glm::vec2 scroll_offset = Input::GetScrollOffset();
	Cam.ProcessMouseScroll(scroll_offset.y);
}

void Teapot::ProcessMousePosition()
{
	_lastOffset = Input::GetMouseOffset();
	if (!Input::IsCursorEnabled()) Cam.ProcessMouseMovement(_lastOffset.x, _lastOffset.y);
}

void Teapot::_init_pipelines()
{
	_skybox = Cubemap({"../Game/Skyboxes/Test/right.jpg","../Game/Skyboxes/Test/left.jpg","../Game/Skyboxes/Test/top.jpg","../Game/Skyboxes/Test/bottom.jpg","../Game/Skyboxes/Test/front.jpg","../Game/Skyboxes/Test/back.jpg"});

	stbi_set_flip_vertically_on_load(true);

	_meshShader = Shader("mesh.vert", "mesh.frag");
	_model = Model((char*)"../Game/Models/Nanosuit/nanosuit.obj");

	glEnable(GL_DEPTH_TEST);
}

void Teapot::Draw()
{
    while (!Application::ShouldTerminate()) {
		ProcessKeyboardState();
		if (Input::GetMouseOffset() != _lastOffset) ProcessMousePosition();
		ProcessScrollState();

		_currentFrame = glfwGetTime();
		_deltaTime = _currentFrame - _lastFrame;

		// ImGui
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		{
            ImGui::Begin("Teapot Configurations");

			ImGui::Checkbox("Wireframe", &RenderVars.wireframe);
            ImGui::ColorEdit3("clear color", (float*)&RenderVars.clear_color);

			ImGui::DragFloat3("light pos", (float*)&RenderVars.light_pos);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		// Render
		glPolygonMode(GL_FRONT_AND_BACK, RenderVars.wireframe ? GL_LINE : GL_FILL);
		glClearColor(RenderVars.clear_color.x, RenderVars.clear_color.y, RenderVars.clear_color.z, RenderVars.clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = Cam.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(Cam.Zoom), (float)Application::GetWindowExtent().x / (float)Application::GetWindowExtent().y, 0.1f, 100000.0f);
		glm::mat4 model = glm::mat4{ 1.0f };

		_meshShader.use();
		_meshShader.setVec3("viewPos", Cam.Position);
		_meshShader.setVec3("lightPos", glm::vec3(RenderVars.light_pos.x, RenderVars.light_pos.y, RenderVars.light_pos.z));
		_meshShader.setMat4("model", model);
		_meshShader.setMat4("view", view);
		_meshShader.setMat4("projection", projection);
		_meshShader.setVec2("iResolution", Application::GetWindowExtent());
		_meshShader.setFloat("iTime", glfwGetTime());
		_model.Draw(_meshShader);

		_skybox.Draw(glm::mat4(glm::mat3(Cam.GetViewMatrix())), projection);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Application::SwapBuffers();
		Application::PollEvents();
	}
}