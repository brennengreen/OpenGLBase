#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <vector>

#include <glm/glm.hpp>

#include "Teapot.h"

int main() {
	Teapot tp_engine;

	tp_engine.draw();

	return 0;
}