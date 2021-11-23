#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "Shader.h"

struct GVertex {
	float x;
	float y;
	float z;
	glm::vec3 normal;
};
struct GTriangle {
	GVertex vertices[3];
	glm::vec3 normal;
};
struct GMesh {
	std::vector<GVertex> vertices;
	std::vector<GTriangle> triangles;
};

class Geometry {
public:
	std::vector<float> mesh;
	std::vector<GLuint> indices;
	std::vector<glm::vec3> normals;
	GMesh testMesh;
	GLuint VAO;
	Shader shader;

	Geometry(std::vector<float> vertices, std::vector<GLuint> indices);
	void Draw();
	void setShader(const char* vs, const char* fs);

private:
	GLuint VBO, EBO;


	void setup();
	void getNormals();

};


#endif // !GEOMETRY_H