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
	ImVec4 light_pos = ImVec4(1.f, 1.f, 1.f, 0.0f);
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