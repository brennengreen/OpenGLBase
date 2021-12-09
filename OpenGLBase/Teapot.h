#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"

class Teapot {
public:
	explicit Teapot(glm::vec2 res): _windowExtent(res) {}
	~Teapot();
	void init();
	void draw();

private:
	void _init_callbacks();
	void _init_pipelines();

	Shader _meshShader;
	GLuint _VBO {0}, _VAO {0}, _EBO {0};

	GLFWwindow * _window = nullptr;
	glm::vec2 _windowExtent;
	int _frameNumber {0};
};

//void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
//	glViewport(0, 0, width, height);
//}
//
//void mouse_callback(GLFWwindow * window, double xpos, double ypos)
//{
//}
//
//void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
//{
//}
//
//
//void process_input(GLFWwindow* window) {
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//}