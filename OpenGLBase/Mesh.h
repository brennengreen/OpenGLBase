#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"

#pragma warning(disable: 26812)

using namespace std;

constexpr auto MAX_BONE_INFLUENCE = 4;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    bool has_diff, has_spec, has_normal, has_height;

    // constructor
    Mesh() = default;
    ~Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, bool _has_diff, bool _has_spec, bool _has_normal, bool _has_height)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        this->has_diff = _has_diff;
        this->has_spec = _has_spec;
        this->has_normal = _has_normal;
        this->has_height = _has_height;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        _setup_mesh();
    }
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        _setup_mesh();
    }

    // render the mesh
    //void Draw(Shader &shader) 
    //{
    //    // bind appropriate textures
    //    unsigned int diffuseNr  = 1;
    //    unsigned int specularNr = 1;
    //    unsigned int normalNr   = 1;
    //    unsigned int heightNr   = 1;
    //    for(unsigned int i = 0; i < textures.size(); i++)
    //    {
    //        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    //        // retrieve texture number (the N in diffuse_textureN)
    //        string number;
    //        string name = textures[i].type;
    //        if(name == "texture_diffuse")
    //            number = std::to_string(diffuseNr++);
    //        else if(name == "texture_specular")
    //            number = std::to_string(specularNr++); // transfer unsigned int to string
    //        else if(name == "texture_normal")
    //            number = std::to_string(normalNr++); // transfer unsigned int to string
    //         else if(name == "texture_height")
    //            number = std::to_string(heightNr++); // transfer unsigned int to string

    //        // now set the sampler to the correct texture unit
    //        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    //        // and finally bind the texture
    //        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //    }
    //    
    //    shader.setBool("has_diff", has_diff);
    //    shader.setBool("has_spec", has_spec);
    //    shader.setBool("has_normal", has_normal);
    //    shader.setBool("has_height", has_height);

    //    // draw mesh
    //    glBindVertexArray(VAO);
    //    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //    glBindVertexArray(0);

    //    // always good practice to set everything back to defaults once configured.
    //    glActiveTexture(GL_TEXTURE0);
    //}
    void Run(Shader &shader) 
    {
        // bind appropriate textures
        unsigned int albedoNr   = 1;
        unsigned int normalNr   = 1;
        unsigned int metalNr    = 1;
        unsigned int roughNr    = 1;
        unsigned int aoNr       = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_albedo")
                number = std::to_string(albedoNr++);
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if(name == "texture_metallic")
                number = std::to_string(metalNr++); // transfer unsigned int to string
            else if(name == "texture_roughness")
                number = std::to_string(roughNr++); // transfer unsigned int to string
            else if(name == "texture_ao")
                number = std::to_string(aoNr++);

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int _VBO, _EBO;

    // initializes all the buffer objects/arrays
    void _setup_mesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &_VBO);
        glGenBuffers(1, &_EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
