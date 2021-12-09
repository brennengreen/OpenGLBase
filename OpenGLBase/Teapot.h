#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"

#include "imgui.h"


static struct TeapotImguiConfigurations {
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float rotate_var = 0;
	bool wireframe = false;
};

class Teapot {
public:
	explicit Teapot(glm::vec2 res): _windowExtent(res) {}
	~Teapot();
	void init();
	void draw();
	
	TeapotImguiConfigurations render_vars;
private:
	void _init_callbacks();
	void _init_pipelines();
	void _init_imgui();

	Shader _meshShader;
	GLuint _VBO {0}, _VAO {0}, _EBO {0};

	GLFWwindow * _window = nullptr;
	glm::vec2 _windowExtent;
	int _frameNumber {0};

private:
	// Callbacks
	static void _framebuffer_size_callback(GLFWwindow * window, int width, int height) {
		glViewport(0, 0, width, height);
	}
	static void _mouse_callback(GLFWwindow * window, double xpos, double ypos);
	static void _scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
	static void _process_input(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
	static void _glfw_error_callback(int error, const char * description) {
		std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
	}
};