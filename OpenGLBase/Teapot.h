#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"

#include "imgui.h"


struct TeapotImguiConfigurations {
	ImVec4 clear_color = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	bool wireframe = false;
};

class Teapot {
public:
	Teapot();
	~Teapot();
	void draw();
	
	Camera cam{};

	TeapotImguiConfigurations render_vars;

	void ProcessKeyboardState();
	void ProcessScrollState();
	void ProcessMousePosition();
private:
	void _init_callbacks();
	void _init_pipelines();
	void _init_imgui();

	Shader _meshShader;
	GLuint _VBO {0}, _VAO {0}, _EBO {0};

	GLFWwindow * _window = nullptr;
	glm::vec2 _windowExtent;

	GLdouble _deltaTime {0};
	GLdouble _currentFrame {0};
	GLdouble _lastFrame {0};

	glm::vec2 _lastOffset {0, 0};

private:
	// Callbacks
	static void _framebuffer_size_callback(GLFWwindow * window, int width, int height) {
		glViewport(0,0,width,height);
	}
	static void _glfw_error_callback(int error, const char * description) {
		std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
	}
};