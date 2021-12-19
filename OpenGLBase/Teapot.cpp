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

void Teapot::ShadowPass()
{
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 1000.0f;
	lightProjection = glm::ortho(-100.0f, -100.0f, -100.0f, -100.0f, near_plane, far_plane);
	glm::vec3 dir_light_pos = glm::vec3(RenderVars.dir_light_pos.x, RenderVars.dir_light_pos.y, RenderVars.dir_light_pos.z);
	lightView = glm::lookAt(dir_light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	lightSpaceMatrix = lightProjection * lightView;

	_shadowShader.use();
	_shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 model = glm::scale(glm::mat4{ 1.0f }, glm::vec3(RenderVars.discard_radius, RenderVars.discard_radius, RenderVars.discard_radius));
		_model.Draw(_shadowShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Teapot::RenderPass()
{
	glViewport(0, 0, Application::GetWindowExtent().x, Application::GetWindowExtent().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render Scene
	RenderVars.light_pos_1 = ImVec4(100.f*glm::cos(_currentFrame/2.0f), 5.0, 0.0, 1.0);
	RenderVars.dir_light_pos = ImVec4(0.0, 700.0f, 600.0f + 400*glm::abs(cos(glfwGetTime() / 10.0f)), 0.0);

	_meshShader.use();


	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 1000.0f;
	lightProjection = glm::ortho(-100.0f, -100.0f, -100.0f, -100.0f, near_plane, far_plane);
	glm::vec3 dir_light_pos = glm::vec3(RenderVars.dir_light_pos.x, RenderVars.dir_light_pos.y, RenderVars.dir_light_pos.z);
	lightView = glm::lookAt(dir_light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	lightSpaceMatrix = lightProjection * lightView;
	_meshShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glm::mat4 view = Cam.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(Cam.Zoom), (float)Application::GetWindowExtent().x / (float)Application::GetWindowExtent().y, 0.1f, 10000.0f);
	glm::mat4 model = glm::scale(glm::mat4{ 1.0f }, glm::vec3(RenderVars.discard_radius, RenderVars.discard_radius, RenderVars.discard_radius));
	_meshShader.setMat4("view", view);
	_meshShader.setMat4("projection", projection);

	// Direction Light
	_meshShader.setVec3("dirLight.direction",  glm::vec3(RenderVars.dir_light_pos.x, RenderVars.dir_light_pos.y, RenderVars.dir_light_pos.z));
    _meshShader.setVec3("dirLight.ambient", glm::vec3(RenderVars.dir_light_amb.x, RenderVars.dir_light_amb.y, RenderVars.dir_light_amb.z));
    _meshShader.setVec3("dirLight.diffuse", glm::vec3(RenderVars.dir_light_diff.x, RenderVars.dir_light_diff.y, RenderVars.dir_light_diff.z));
    _meshShader.setVec3("dirLight.specular", glm::vec3(RenderVars.dir_light_spec.x, RenderVars.dir_light_spec.y, RenderVars.dir_light_spec.z));
    // point light 1
    _meshShader.setVec3("pointLights[0].position", glm::vec3(RenderVars.light_pos_1.x, RenderVars.light_pos_1.y, RenderVars.light_pos_1.z));
    _meshShader.setVec3("pointLights[0].ambient", glm::vec3(RenderVars.light_amb_1.x, RenderVars.light_amb_1.y, RenderVars.light_amb_1.z));
    _meshShader.setVec3("pointLights[0].diffuse", glm::vec3(RenderVars.light_diff_1.x, RenderVars.light_diff_1.y, RenderVars.light_diff_1.z));
    _meshShader.setVec3("pointLights[0].specular", glm::vec3(RenderVars.light_spec_1.x, RenderVars.light_spec_1.y, RenderVars.light_spec_1.z));
    _meshShader.setFloat("pointLights[0].constant", 1.0f);
    _meshShader.setFloat("pointLights[0].linear", RenderVars.light_linear_1);
    _meshShader.setFloat("pointLights[0].quadratic", RenderVars.light_quadratic_1);
    // point light 2
    _meshShader.setVec3("pointLights[1].position", glm::vec3(RenderVars.light_pos_2.x, RenderVars.light_pos_2.y, RenderVars.light_pos_2.z));
    _meshShader.setVec3("pointLights[1].ambient", glm::vec3(RenderVars.light_amb_2.x, RenderVars.light_amb_2.y, RenderVars.light_amb_2.z));
    _meshShader.setVec3("pointLights[1].diffuse", glm::vec3(RenderVars.light_diff_2.x, RenderVars.light_diff_2.y, RenderVars.light_diff_2.z));
    _meshShader.setVec3("pointLights[1].specular", glm::vec3(RenderVars.light_spec_2.x, RenderVars.light_spec_2.y, RenderVars.light_spec_2.z));
    _meshShader.setFloat("pointLights[1].constant", 1.0f);
    _meshShader.setFloat("pointLights[1].linear",  RenderVars.light_linear_2);
    _meshShader.setFloat("pointLights[1].quadratic", RenderVars.light_quadratic_2);
    // point light 3
    _meshShader.setVec3("pointLights[2].position", glm::vec3(RenderVars.light_pos_3.x, RenderVars.light_pos_3.y, RenderVars.light_pos_3.z));
    _meshShader.setVec3("pointLights[2].ambient", glm::vec3(RenderVars.light_amb_3.x, RenderVars.light_amb_3.y, RenderVars.light_amb_3.z));
    _meshShader.setVec3("pointLights[2].diffuse", glm::vec3(RenderVars.light_diff_3.x, RenderVars.light_diff_3.y, RenderVars.light_diff_3.z));
    _meshShader.setVec3("pointLights[2].specular", glm::vec3(RenderVars.light_spec_3.x, RenderVars.light_spec_3.y, RenderVars.light_spec_3.z));
    _meshShader.setFloat("pointLights[2].constant", 1.0f);
    _meshShader.setFloat("pointLights[2].linear",  RenderVars.light_linear_3);
    _meshShader.setFloat("pointLights[2].quadratic", RenderVars.light_quadratic_3);
    // point light 4
    _meshShader.setVec3("pointLights[3].position", glm::vec3(RenderVars.light_pos_4.x, RenderVars.light_pos_4.y, RenderVars.light_pos_4.z));
    _meshShader.setVec3("pointLights[3].ambient", glm::vec3(RenderVars.light_amb_4.x, RenderVars.light_amb_4.y, RenderVars.light_amb_4.z));
    _meshShader.setVec3("pointLights[3].diffuse", glm::vec3(RenderVars.light_diff_4.x, RenderVars.light_diff_4.y, RenderVars.light_diff_4.z));
    _meshShader.setVec3("pointLights[3].specular", glm::vec3(RenderVars.light_spec_4.x, RenderVars.light_spec_4.y, RenderVars.light_spec_4.z));
    _meshShader.setFloat("pointLights[3].constant", 1.0f);
    _meshShader.setFloat("pointLights[3].linear",  RenderVars.light_linear_4);
    _meshShader.setFloat("pointLights[3].quadratic", RenderVars.light_quadratic_4);

	_meshShader.setVec3("viewPos", Cam.Position);
	_meshShader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthTexture);

	_model.Draw(_meshShader);

	_skybox.Draw(glm::mat4(glm::mat3(Cam.GetViewMatrix())), projection);
}

void Teapot::ImGuiPass() {
	ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	{
        ImGui::Begin("Teapot Configurations");

		ImGui::Checkbox("Wireframe", &RenderVars.wireframe);
        ImGui::ColorEdit3("clear color", (float*)&RenderVars.clear_color);
		ImGui::InputFloat("Discard", (float*)&RenderVars.discard_radius);

		ImGui::Text("Direction Light Settings");
		ImGui::DragFloat3("Position", (float*)&RenderVars.dir_light_pos);
		ImGui::ColorEdit3("Ambient", (float*)&RenderVars.dir_light_amb);
		ImGui::ColorEdit3("Diffuse", (float*)&RenderVars.dir_light_diff);
		ImGui::ColorEdit3("Specular", (float*)&RenderVars.dir_light_spec);
			
		ImGui::Text("Point Light 1 Settings");
		ImGui::DragFloat3("Position 1", (float*)&RenderVars.light_pos_1);
		ImGui::ColorEdit3("Ambient 1", (float*)&RenderVars.light_amb_1);
		ImGui::ColorEdit3("Diffuse 1", (float*)&RenderVars.light_diff_1);
		ImGui::ColorEdit3("Specular 1", (float*)&RenderVars.light_spec_1);
		ImGui::InputFloat("Linear 1", (float*)&RenderVars.light_linear_1);
		ImGui::InputFloat("Quadratic 1", (float*)&RenderVars.light_quadratic_1);
			
		ImGui::Text("Point Light 2 Settings");
		ImGui::DragFloat3("Position 2", (float*)&RenderVars.light_pos_2);
		ImGui::ColorEdit3("Ambient 2", (float*)&RenderVars.light_amb_2);
		ImGui::ColorEdit3("Diffuse 2", (float*)&RenderVars.light_diff_2);
		ImGui::ColorEdit3("Specular 2", (float*)&RenderVars.light_spec_2);
		ImGui::InputFloat("Linear 2", (float*)&RenderVars.light_linear_2);
		ImGui::InputFloat("Quadratic 2", (float*)&RenderVars.light_quadratic_2);

		ImGui::Text("Point Light 3 Settings");
		ImGui::DragFloat3("Position 3", (float*)&RenderVars.light_pos_3);
		ImGui::ColorEdit3("Ambient 3", (float*)&RenderVars.light_amb_3);
		ImGui::ColorEdit3("Diffuse 3", (float*)&RenderVars.light_diff_3);
		ImGui::ColorEdit3("Specular 3", (float*)&RenderVars.light_spec_3);
		ImGui::InputFloat("Linear 3", (float*)&RenderVars.light_linear_3);
		ImGui::InputFloat("Quadratic 3", (float*)&RenderVars.light_quadratic_3);

		ImGui::Text("Point Light 4 Settings");
		ImGui::DragFloat3("Position 4", (float*)&RenderVars.light_pos_4);
		ImGui::ColorEdit3("Ambient 4", (float*)&RenderVars.light_amb_4);
		ImGui::ColorEdit3("Diffuse 4", (float*)&RenderVars.light_diff_4);
		ImGui::ColorEdit3("Specular 4", (float*)&RenderVars.light_spec_4);
		ImGui::InputFloat("Linear 4", (float*)&RenderVars.light_linear_4);
		ImGui::InputFloat("Quadratic 4", (float*)&RenderVars.light_quadratic_4);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	glEnable(GL_DEPTH_TEST);

	_skybox = Cubemap({"../Game/Skyboxes/Test/right.jpg","../Game/Skyboxes/Test/left.jpg","../Game/Skyboxes/Test/top.jpg","../Game/Skyboxes/Test/bottom.jpg","../Game/Skyboxes/Test/front.jpg","../Game/Skyboxes/Test/back.jpg"});

	stbi_set_flip_vertically_on_load(true);

	_meshShader = Shader("Shaders/mesh.vert", "Shaders/mesh.frag");
	_model = Model((char*)"../Game/Models/Sponza/sponza.obj");

	_shadowShader = Shader("Shaders/shadow.vert", "Shaders/shadow.frag");

	glGenFramebuffers(1, &_FBO);
	glGenTextures(1, &_depthTexture);
	glBindTexture(GL_TEXTURE_2D, _depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Teapot::Draw()
{
    while (!Application::ShouldTerminate()) {
		ProcessKeyboardState();
		if (Input::GetMouseOffset() != _lastOffset) ProcessMousePosition();
		ProcessScrollState();

		_currentFrame = glfwGetTime();
		_deltaTime = _currentFrame - _lastFrame;

		// Render
		glPolygonMode(GL_FRONT_AND_BACK, RenderVars.wireframe ? GL_LINE : GL_FILL);
		glClearColor(RenderVars.clear_color.x, RenderVars.clear_color.y, RenderVars.clear_color.z, RenderVars.clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ShadowPass();
		RenderPass();
		ImGuiPass();

		Application::SwapBuffers();
		Application::PollEvents();
	}
}