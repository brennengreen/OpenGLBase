#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <vector>

#include <glm/glm.hpp>

#include "Teapot.h"

glm::vec2 resolution(800, 600);

int main() {
	Teapot tp_engine(resolution);

	tp_engine.init();

	tp_engine.draw();

	return 0;
}