#include "Geometry.h"

Geometry::Geometry(std::vector<float> vertices, std::vector<GLuint> indices)
{
	mesh = vertices;
	this->indices = indices;

	for (int i = 0; i < indices.size(); i = i + 3)
	{
		std::cout << "Triangle: ";
		std::cout << "X: " << vertices[indices[i]];
		std::cout << "Y: " << vertices[indices[i + 1]];
		std::cout << "Z: " << vertices[indices[i + 2]] << std::endl;
		float x = vertices[indices[i]];
		float y = vertices[indices[i]];
		float z = vertices[indices[i]];
		//testMesh.vertices.push_back(Vertex(x, y, z))
	}

	setup();
}

void Geometry::Draw()
{
	shader.use();
	glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Geometry::setShader(const char* vs, const char* fs)
{
	shader = Shader(vs, fs);
}

void Geometry::setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), &mesh[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Geometry::getNormals()
{
}