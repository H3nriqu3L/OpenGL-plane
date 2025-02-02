#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "Mesh.h"

class Model
{
public:
    // Constructor to load a .obj file
    Model(const char* file, const char* text);

    void Draw(Shader& shader, Camera& camera);

private:
    // Store all the meshes
    std::vector<Mesh> meshes;
    const char* texture;
    // Load .obj file
    void loadOBJ(const char* file, const char* text);

};

Model::Model(const char* file, const char* text)
{
    // Load the .obj file
    loadOBJ(file, text);

}

void Model::Draw(Shader& shader, Camera& camera)
{
    // Draw all meshes
    for (auto& mesh : meshes)
    {
        mesh.Draw(shader, camera);
    }
}

void Model::loadOBJ(const char* file, const char* text)
{
    // Temporary containers
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texUVs;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::ifstream in(file);
    if (!in)
    {
        std::cerr << "Cannot open the file: " << file << std::endl;
        return;
    }

    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") // Vertex positions
        {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (type == "vt") // Texture coordinates
        {
            glm::vec2 texUV;
            ss >> texUV.x >> texUV.y;
            texUVs.push_back(texUV);
        }
        else if (type == "vn") // Vertex normals
        {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (type == "f") // Faces
        {
            GLuint vertexIndex[3], texUVIndex[3], normalIndex[3];
            char slash;

            for (int i = 0; i < 3; i++)
            {
                ss >> vertexIndex[i] >> slash >> texUVIndex[i] >> slash >> normalIndex[i];
                Vertex vertex;
                vertex.position = positions[vertexIndex[i] - 1];
                vertex.texUV = texUVs[texUVIndex[i] - 1];
                vertex.normal = normals[normalIndex[i] - 1];
                vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    // Create the mesh
    const char* t = text;
    Texture textures[]
    {
       
        Texture(t, "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE)
    };

    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

    meshes.emplace_back(vertices, indices, tex);

}

#endif