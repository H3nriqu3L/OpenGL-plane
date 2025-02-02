#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include "shaderClass.h"
#include "VAO.h"
#include<glm/gtc/type_ptr.hpp>
#include <filesystem>

const unsigned int width_sky = 800;
const unsigned int height_sky = 800;

class Skybox {
private:
    VAO skyboxVAO;

    unsigned int cubemapTexture;

    void load_texture();

public:
    Skybox();
    ~Skybox();
    void draw(Shader &shader,  Camera &camera);
};

GLfloat skyboxVertices[] =
{
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
};

GLuint skyboxIndices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    //0, 3, 2,
    //2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};


std::string morningFaces[6] = {
    "../../texture/skybox/right.jpg",
    "../../texture/skybox/left.jpg",
    "../../texture/skybox/top.jpg",
    "../../texture/skybox/bottom.jpg",
    "../../texture/skybox/front.jpg",
    "../../texture/skybox/back.jpg"
};


Skybox::Skybox() 
{

    VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    EBO skyboxEBO(skyboxIndices, sizeof(skyboxIndices));

    skyboxVAO.Bind();
    skyboxVBO.Bind();
    skyboxEBO.Bind();

    skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    skyboxVAO.Unbind();
    skyboxVBO.Unbind();
    skyboxEBO.Unbind();


    load_texture();


}


Skybox::~Skybox() {

    skyboxVAO.Delete();

}


void Skybox::load_texture() {


    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(morningFaces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << morningFaces[i] << std::endl;
            stbi_image_free(data);
        }
    }
}


void Skybox::draw(Shader &shader, Camera &camera) {
    glDepthFunc(GL_LEQUAL);
    shader.Activate();

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
    projection = glm::perspective(glm::radians(45.0f),(float)width_sky / height_sky, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(skyboxVAO.ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

}

#endif