#include "Teapot.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Teapot::~Teapot() {
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);

	glfwTerminate();
}

void Teapot::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	_init_callbacks();
	_init_pipelines();
}

void Teapot::_init_callbacks()
{
	//glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
}

void Teapot::_init_pipelines()
{
	_meshShader = Shader("mesh.vert", "mesh.frag");

    float vertices[] = {
         1.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

	glEnable(GL_DEPTH_TEST);
}

void Teapot::draw()
{
    while (!glfwWindowShouldClose(_window)) {
		// Process Input
		//process_input(_window);
		// Render
		glClearColor(0.66f, 0.79f, 0.85f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_meshShader.use();

		glm::vec3 camPos = {0.f, 0.f, -2.f};
		glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
		glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_windowExtent.x / _windowExtent.y, 0.1f, 200.0f);
		projection[1][1] *= -1;
		glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(_frameNumber++ * 0.1f), glm::vec3(0, 1, 0));


		_meshShader.setMat4("model", model);
		_meshShader.setMat4("view", view);
		_meshShader.setMat4("projection", projection);
		_meshShader.setVec2("iResolution", _windowExtent);
		_meshShader.setFloat("iTime", glfwGetTime());


		glBindVertexArray(_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}