#pragma once

#include "Mesh.h"

#include <map>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma warning(disable: 26812)


unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model
{
public:
	Model() = default;
	Model(char * path)
	{
		_load_model(path);
	}
	void Draw(Shader &shader);
private:
	std::vector<Mesh> _meshes;
	std::string _directory;
	std::vector<Texture> _textures_loaded;

	void _load_model(std::string path);
	void _process_node(aiNode * node, const aiScene * scene);
	Mesh _process_mesh(aiMesh * mesh, const aiScene * scene);
	std::vector<Texture> _load_material_textures(aiMaterial * mat, aiTextureType type, std::string typeName);

};