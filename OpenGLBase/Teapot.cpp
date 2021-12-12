#include "Teapot.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Input.h"

#include "stb_image.h"

Teapot::Teapot() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_windowExtent = glm::vec2(1920, 1080);
	_window = glfwCreateWindow(_windowExtent.x, _windowExtent.y, "Teapot Engine", NULL, NULL);
	if (!_window) {
		std::cerr << "Failed to create GLFW Window!" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(_window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, _windowExtent.x, _windowExtent.y);

	Input::Update();
	_init_callbacks();
	_init_pipelines();
	_init_imgui();
}

Teapot::~Teapot() {
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	/*glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);*/

	glfwTerminate();
}

void Teapot::_init_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Teapot::ProcessKeyboardState()
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Input::SetCursorEnabled(true);
	if (Input::IsKeyPressed(GLFW_KEY_ENTER))
		Input::SetCursorEnabled(false);
	if(Input::IsKeyPressed(GLFW_KEY_W) && !Input::IsCursorEnabled())
		cam.ProcessKeyboard(CameraMovement::FORWARD, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_A) && !Input::IsCursorEnabled())
		cam.ProcessKeyboard(CameraMovement::LEFT, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_S) && !Input::IsCursorEnabled())
		cam.ProcessKeyboard(CameraMovement::BACKWARD, _deltaTime);
	if(Input::IsKeyPressed(GLFW_KEY_D) && !Input::IsCursorEnabled())
		cam.ProcessKeyboard(CameraMovement::RIGHT, _deltaTime);
}

void Teapot::ProcessScrollState()
{
	glm::vec2 scroll_offset = Input::GetScrollOffset();
	cam.ProcessMouseScroll(scroll_offset.y);
}

void Teapot::ProcessMousePosition()
{
	_lastOffset = Input::GetMouseOffset();
	if (!Input::IsCursorEnabled()) cam.ProcessMouseMovement(_lastOffset.x, _lastOffset.y);
}

void Teapot::_init_callbacks()
{
	glfwSetErrorCallback(_glfw_error_callback);
	glfwSetFramebufferSizeCallback(_window, _framebuffer_size_callback);
	Input::init_glfw_input_callbacks(_window);
}

void Teapot::_init_pipelines()
{
	stbi_set_flip_vertically_on_load(true);

	_meshShader = Shader("mesh.vert", "mesh.frag");
	_model = Model((char*)"../Game/Models/Sponza/sponza.obj");

	glEnable(GL_DEPTH_TEST);
}

void Teapot::draw()
{
    while (!glfwWindowShouldClose(_window)) {
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

			ImGui::Checkbox("Wireframe", &render_vars.wireframe);
            ImGui::ColorEdit3("clear color", (float*)&render_vars.clear_color);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		// Render
		glPolygonMode(GL_FRONT_AND_BACK, render_vars.wireframe ? GL_LINE : GL_FILL);
		glClearColor(render_vars.clear_color.x, render_vars.clear_color.y, render_vars.clear_color.z, render_vars.clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_meshShader.use();

		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)_windowExtent.x / (float)_windowExtent.y, 0.1f, 10000.0f);
		glm::mat4 model = glm::mat4{ 1.0f };

		_meshShader.setMat4("model", model);
		_meshShader.setMat4("view", view);
		_meshShader.setMat4("projection", projection);
		_meshShader.setVec2("iResolution", _windowExtent);
		_meshShader.setFloat("iTime", glfwGetTime());

		_model.Draw(_meshShader);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}