#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Cubemap.h"
#include "imgui.h"


struct TeapotImguiConfigurations {
	ImVec4 clear_color = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	bool wireframe = false;

	ImVec4 dir_light_pos = ImVec4(500.f, 1500.f, 56.f, 0.0f);
	ImVec4 light_pos_1 = ImVec4(200.f, 5.f, 200.f, 0.0f);
	ImVec4 light_pos_2 = ImVec4(-200.f, 5.f, 200.f, 0.0f);
	ImVec4 light_pos_3 = ImVec4(200.f, 5.f, -200.f, 0.0f);
	ImVec4 light_pos_4 = ImVec4(-200.f, 5.f, -200.f, 0.0f);


	ImVec4 dir_light_amb = ImVec4(0.05f, 0.05f, 0.05f, 0.0f);
	ImVec4 light_amb_1 = ImVec4(1.f, 1.f, 1.f, 0.0f);
	ImVec4 light_amb_2 = ImVec4(1.f, 0.f, 0.f, 0.0f);
	ImVec4 light_amb_3 = ImVec4(0.f, 1.f, 0.f, 0.0f);
	ImVec4 light_amb_4 = ImVec4(0.f, 0.f, 1.f, 0.0f);

	ImVec4 dir_light_diff = ImVec4(0.4f, 0.4f, 0.4f, 0.0f);
	ImVec4 light_diff_1 = ImVec4(0.f, 0.f, 0.f, 0.0f);
	ImVec4 light_diff_2 = ImVec4(0.f, 0.f, 0.f, 0.0f);
	ImVec4 light_diff_3 = ImVec4(0.f, 0.f, 0.f, 0.0f);
	ImVec4 light_diff_4 = ImVec4(0.f, 0.f, 0.f, 0.0f);

	ImVec4 dir_light_spec = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	ImVec4 light_spec_1 = ImVec4(1.f, 1.f, 1.f, 0.0f);
	ImVec4 light_spec_2 = ImVec4(1.f, 0.f, 0.f, 0.0f);
	ImVec4 light_spec_3 = ImVec4(0.f, 1.f, 0.f, 0.0f);
	ImVec4 light_spec_4 = ImVec4(0.f, 0.f, 1.f, 0.0f);

	float light_linear_1 = 0.014;
	float light_linear_2 = 1.0;
	float light_linear_3 = 1.0;
	float light_linear_4 = 1.0;

	float light_quadratic_1 = 0.000007;
	float light_quadratic_2 = 1.0;
	float light_quadratic_3 = 1.0;
	float light_quadratic_4 = 1.0;
};

class Teapot {
public:
	Teapot();
	~Teapot();
	void Draw();
	
	Camera Cam{};
	TeapotImguiConfigurations RenderVars;

	void ProcessKeyboardState();
	void ProcessScrollState();
	void ProcessMousePosition();
private:
	void _init_pipelines();
	void _init_imgui();

	Shader _meshShader;
	Model _model;

	Cubemap _skybox;

	Shader _shadowShader;

	unsigned int _depth_FBO;
    unsigned int _depth_map;

	GLdouble _deltaTime {0};
	GLdouble _currentFrame {0};
	GLdouble _lastFrame {0};

	glm::vec2 _lastOffset {0, 0};

private:
	unsigned int _load_cubemap(std::vector<std::string> faces);

};