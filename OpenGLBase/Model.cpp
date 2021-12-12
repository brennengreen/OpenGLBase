#include "Model.h"

#include "stb_image.h"

#pragma warning(disable: 26812)

void Model::Draw(Shader& shader)
{
	for(unsigned int i = 0; i < _meshes.size(); i++)
		_meshes[i].Draw(shader);
}

void Model::_load_model(std::string path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	_directory = path.substr(0, path.find_last_of('/'));

	_process_node(scene->mRootNode, scene);
}

void Model::_process_node(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(_process_mesh(mesh, scene));
	}

	for(unsigned int i = 0; i < node->mNumChildren; i++) {
		_process_node(node->mChildren[i], scene);
	}
}

Mesh Model::_process_mesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		vertex.Normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);
		if(mesh->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		} else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = _load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");

		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = _load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");

		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::_load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i , &str);
		bool skip = false;
		for(unsigned int j = 0; j < _textures_loaded.size(); j++)
        {
            if(std::strcmp(_textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(_textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

		if (!skip) {
			Texture texture;
            texture.id = TextureFromFile(str.C_Str(), _directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            _textures_loaded.push_back(texture);
		}
	}
	return textures;

}

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
