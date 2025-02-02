
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE

#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include "Mesh.h"
#include "Model.h"
#include "json.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "skybox.h"

// Add these globals at the top of your code
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
unsigned int depthMapFBO;
unsigned int depthMap;

using namespace std;

GLint width = 800, height = 800;
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float alpha)
{
	return start + alpha * (end - start);
}




int main(void)
{



    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	glViewport(0, 0, 800, 800);


	Shader shaderProgram("../../src/shaders/shader.vert", "../../src/shaders/shader.frag");
	Shader shaderSkyBox("../../src/shaders/skybox.vs", "../../src/shaders/skybox.fs");
	Shader shadowMapProgram("../../src/shaders/shadowMap.vert", "../../src/shaders/shadowMap.frag");

	Skybox sky = Skybox();

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 objectModel = glm::mat4(1.0f);
	
	shaderProgram.Activate();


	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	Model cce("../../models/cce.obj", "../../texture/texturacce.png");
	Model corpo("../../models/corpo.obj", "../../texture/texturebr.png");

	Model turbina1("../../models/turbina1.obj", "../../texture/texturebr.png");
	Model turbina2("../../models/turbina2.obj", "../../texture/texturebr.png");

	Model helice1("../../models/helice1.obj", "../../texture/texturebr.png");
	Model helice2("../../models/helice2.obj", "../../texture/texturebr.png");

	Model chao("../../models/mountain.obj", "../../texture/mountain.png");

	Model gravel("../../models/gravel.obj", "../../texture/gravel.png");
	


	glm::mat4 heliceModel1 = glm::mat4(1.0f);
	//glm::vec3 helice1Pos( 0.0f, 0.0f, 0.0f);

	glm::vec3 helice1Pos(-1.574f, 5.908f, -7.041f);
	glm::mat4 heliceModel2 = glm::mat4(1.0f);
	glm::vec3 helice2Pos(-1.574f, 5.908f, 7.041f);
	glm::mat4 chaoModel = glm::mat4(1.0f);
	glm::mat4 cceModel = glm::mat4(1.0f);
	glm::mat4 gravelModel = glm::mat4(1.0f);
	glm::mat4 skyModel = glm::mat4(1.0f);

	glm::mat4 turbinaModel1 = glm::mat4(1.0f);
	glm::mat4 turbinaModel2 = glm::mat4(1.0f);
	glm::vec3 turbina1Pos(-1.574f, 3.308f, -7.041f);
	glm::vec3 turbina2Pos(-1.574f, 3.308f, 7.041f);
 

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


	// Creates camera object              (x, z, y)
	Camera camera(width, height, glm::vec3(27.0f, 6.0f, 0.0f));
	glm::vec3 cameraOffset = glm::vec3(27.0f, 6.0f, 0.0f);
	glm::vec3 cameraOrientationOffset = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera.Orientation = glm::vec3(-1.0f, 0.0f, 0.0f);

	float cameraLagFactor = 0.01f;
	
	camera.updateMatrix(45.0f, 0.1f, 100.0f);

	Camera camera2(width, height, glm::vec3(27.0f, 6.0f, 0.0f));

	// Main while loop
	glm::vec3 targetOrientation = camera.Orientation;
	glm::vec3 targetCameraPosition = camera.Position;

	float FOVdeg = 70.0f, nearPlane = 0.1f, farPlane = 10000.f;

	float rotationHeliceAngle = 0.0f;
	float rotation = 0.0f;
	float heliceRotation = 0.0f;
	float rotationSpeed = 50.0f;

	bool isHorizontal = false;
	bool isVertical = true;

	double prevTime = glfwGetTime();







	//------------------------------------- NEW------------------------------------------- //

	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// Handles camera inputs
		camera.Inputs(window);
		camera.updateMatrix(FOVdeg, nearPlane, farPlane);
	

		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			
			if(rotationHeliceAngle>=0 && rotationHeliceAngle<5000)
				rotationHeliceAngle += 0.0005f;
			else if(rotationHeliceAngle > -5000)
				rotationHeliceAngle -= 0.0005f;
			prevTime = crntTime;
		}

		



		// Rotate Helices
		heliceModel1 = glm::translate(heliceModel1, helice1Pos);
		heliceModel1 = glm::rotate(heliceModel1, glm::radians(rotationHeliceAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		heliceModel1 = glm::translate(heliceModel1, -helice1Pos);

		heliceModel2 = glm::translate(heliceModel2, helice2Pos);
		heliceModel2 = glm::rotate(heliceModel2, glm::radians(-rotationHeliceAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		heliceModel2 = glm::translate(heliceModel2, -helice2Pos);

		
		
		// Extrai os eixos locais do avião a partir da matriz objectModel
		glm::vec3 front = -glm::normalize(glm::vec3(objectModel[0])); // Eixo Z (frente)
		glm::vec3 right = glm::normalize(glm::vec3(objectModel[0])); // Eixo X (direita)
		glm::vec3 up = glm::normalize(glm::vec3(objectModel[1]));    // Eixo Y (cima)

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			if (!isHorizontal) {
				float rot = 0.5f;
				heliceRotation += rot;

				if (heliceRotation >= 90.0f) {
					isHorizontal = true;
					isVertical = false;
				}

				glm::vec3 afront = glm::normalize(glm::vec3(objectModel[2])); // Z axis
			

				glm::vec3 turbina1GlobalPos = glm::vec3(objectModel * glm::vec4(turbina1Pos, 1.0f));
				glm::vec3 turbina2GlobalPos = glm::vec3(objectModel * glm::vec4(turbina2Pos, 1.0f));
				glm::vec3 aviaoPos = glm::vec3(objectModel[3]);
				// Use right vector as rotation axis since we want to rotate around the wings' axis
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rot), afront );


				heliceModel1 = glm::translate(glm::mat4(1.0f), -turbina1GlobalPos) * heliceModel1;
				heliceModel1 = rotationMatrix * heliceModel1;
				heliceModel1 = glm::translate(glm::mat4(1.0f), turbina1GlobalPos) * heliceModel1;

				heliceModel2 = glm::translate(glm::mat4(1.0f), -turbina2GlobalPos) * heliceModel2;
				heliceModel2 = rotationMatrix * heliceModel2;
				heliceModel2 = glm::translate(glm::mat4(1.0f), turbina2GlobalPos) * heliceModel2;
			
				turbinaModel1 = glm::translate(glm::mat4(1.0f), -turbina1GlobalPos) * turbinaModel1;
				turbinaModel1 = rotationMatrix * turbinaModel1;
				turbinaModel1 = glm::translate(glm::mat4(1.0f), turbina1GlobalPos) * turbinaModel1;

				
				turbinaModel2 = glm::translate(glm::mat4(1.0f), -turbina2GlobalPos) * turbinaModel2;
				turbinaModel2 = rotationMatrix * turbinaModel2;
				turbinaModel2 = glm::translate(glm::mat4(1.0f), turbina2GlobalPos) * turbinaModel2;
			}
		}


		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			if (!isVertical) {
				float rot = -0.5f;
				heliceRotation += rot;

				if (heliceRotation <= 0.0f) {
					isHorizontal = false;
					isVertical = true;
				}

				
				// Calcula posição global das turbinas considerando a rotação do objeto
				glm::vec3 turbina1GlobalPos = glm::vec3(objectModel * glm::vec4(turbina1Pos, 1.0f));
				glm::vec3 turbina2GlobalPos = glm::vec3(objectModel * glm::vec4(turbina2Pos, 1.0f));
				glm::vec3 aviaoPos = glm::vec3(objectModel[3]);


				glm::vec3 afront = glm::normalize(glm::vec3(objectModel[2])); // Z axis
				
				// Matriz de rotação global (em torno do eixo Z)
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rot), afront);

				// Aplicar transformação correta considerando a coordenada global das turbinas
				heliceModel1 = glm::translate(glm::mat4(1.0f), -turbina1GlobalPos) * heliceModel1;
				heliceModel1 = rotationMatrix * heliceModel1;
				heliceModel1 = glm::translate(glm::mat4(1.0f), turbina1GlobalPos) * heliceModel1;

				heliceModel2 = glm::translate(glm::mat4(1.0f), -turbina2GlobalPos) * heliceModel2;
				heliceModel2 = rotationMatrix * heliceModel2;
				heliceModel2 = glm::translate(glm::mat4(1.0f), turbina2GlobalPos) * heliceModel2;

				turbinaModel1 = glm::translate(glm::mat4(1.0f), -turbina1GlobalPos) * turbinaModel1;
				turbinaModel1 = rotationMatrix * turbinaModel1;
				turbinaModel1 = glm::translate(glm::mat4(1.0f), turbina1GlobalPos) * turbinaModel1;

				turbinaModel2 = glm::translate(glm::mat4(1.0f), -turbina2GlobalPos) * turbinaModel2;
				turbinaModel2 = rotationMatrix * turbinaModel2;
				turbinaModel2 = glm::translate(glm::mat4(1.0f), turbina2GlobalPos) * turbinaModel2;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			// Move para cima no espaço local
			glm::vec3 moveDirection = up * 0.04f;
			objectModel = glm::translate(glm::mat4(1.0f), moveDirection) * objectModel;
			heliceModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel1;
			heliceModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel2;
			turbinaModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel1;
			turbinaModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel2;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			// Move para baixo no espaço local
			glm::vec3 moveDirection = -up * 0.04f;
			glm::mat4 aux = glm::translate(glm::mat4(1.0f), moveDirection) * objectModel;

			if (aux[3][1] >= 0) {
				objectModel = glm::translate(glm::mat4(1.0f), moveDirection) * objectModel;
				heliceModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel1;
				heliceModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel2;
				turbinaModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel1;
				turbinaModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel2;
			}

			cout << "x" << objectModel[3][0] << " y " << objectModel[3][2] << " z " << objectModel[3][1] << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			// Move para frente no espaço local
			glm::vec3 moveDirection = front * 0.2f; 
			objectModel = glm::translate(glm::mat4(1.0f), moveDirection) * objectModel;
			heliceModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel1;
			heliceModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * heliceModel2;
			turbinaModel1 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel1;
			turbinaModel2 = glm::translate(glm::mat4(1.0f), moveDirection) * turbinaModel2;
		}

		

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			rotation = 0.3f;
			glm::vec3 aviaoPos = glm::vec3(objectModel[3]);

			// Rotaciona o objeto e suas partes
			objectModel = glm::translate(glm::mat4(1.0f), -aviaoPos) * objectModel;
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			objectModel = rotationMatrix * objectModel;
			objectModel = glm::translate(glm::mat4(1.0f), aviaoPos) * objectModel;

			// Rotaciona as hélices e turbinas em torno do centro do avião
			heliceModel1 = glm::translate(glm::mat4(1.0f), -aviaoPos) * heliceModel1;
			heliceModel1 = rotationMatrix * heliceModel1;
			heliceModel1 = glm::translate(glm::mat4(1.0f), aviaoPos) * heliceModel1;

			heliceModel2 = glm::translate(glm::mat4(1.0f), -aviaoPos) * heliceModel2;
			heliceModel2 = rotationMatrix * heliceModel2;
			heliceModel2 = glm::translate(glm::mat4(1.0f), aviaoPos) * heliceModel2;

			turbinaModel1 = glm::translate(glm::mat4(1.0f), -aviaoPos) * turbinaModel1;
			turbinaModel1 = rotationMatrix * turbinaModel1;
			turbinaModel1 = glm::translate(glm::mat4(1.0f), aviaoPos) * turbinaModel1;

			turbinaModel2 = glm::translate(glm::mat4(1.0f), -aviaoPos) * turbinaModel2;
			turbinaModel2 = rotationMatrix * turbinaModel2;
			turbinaModel2 = glm::translate(glm::mat4(1.0f), aviaoPos) * turbinaModel2;

			// Rotaciona o offset da câmera e a orientação
			cameraOffset = glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));
			camera.Orientation = glm::vec3(rotationMatrix * glm::vec4(camera.Orientation, 1.0f));
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			rotation = -0.3f;
			glm::vec3 aviaoPos = glm::vec3(objectModel[3]);

			// Rotaciona o objeto e suas partes
			objectModel = glm::translate(glm::mat4(1.0f), -aviaoPos) * objectModel;
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			objectModel = rotationMatrix * objectModel;
			objectModel = glm::translate(glm::mat4(1.0f), aviaoPos) * objectModel;

			// Rotaciona as hélices e turbinas em torno do centro do avião
			heliceModel1 = glm::translate(glm::mat4(1.0f), -aviaoPos) * heliceModel1;
			heliceModel1 = rotationMatrix * heliceModel1;
			heliceModel1 = glm::translate(glm::mat4(1.0f), aviaoPos) * heliceModel1;

			heliceModel2 = glm::translate(glm::mat4(1.0f), -aviaoPos) * heliceModel2;
			heliceModel2 = rotationMatrix * heliceModel2;
			heliceModel2 = glm::translate(glm::mat4(1.0f), aviaoPos) * heliceModel2;

			turbinaModel1 = glm::translate(glm::mat4(1.0f), -aviaoPos) * turbinaModel1;
			turbinaModel1 = rotationMatrix * turbinaModel1;
			turbinaModel1 = glm::translate(glm::mat4(1.0f), aviaoPos) * turbinaModel1;

			turbinaModel2 = glm::translate(glm::mat4(1.0f), -aviaoPos) * turbinaModel2;
			turbinaModel2 = rotationMatrix * turbinaModel2;
			turbinaModel2 = glm::translate(glm::mat4(1.0f), aviaoPos) * turbinaModel2;

			// Rotaciona o offset da câmera e a orientação
			cameraOffset = glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));
			camera.Orientation = glm::vec3(rotationMatrix * glm::vec4(camera.Orientation, 1.0f));
		}

		// Atualiza a posição da câmera
		targetCameraPosition = glm::vec3(objectModel[3][0], objectModel[3][1], objectModel[3][2]) + cameraOffset;
		if (camera.Position != targetCameraPosition) {
			camera.Position = lerp(camera.Position, targetCameraPosition, cameraLagFactor);
		}


		glDepthFunc(GL_LEQUAL);  
		sky.draw(shaderSkyBox, camera);
		glDepthFunc(GL_LESS);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cceModel));
		cce.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
		corpo.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(heliceModel1));
		helice1.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(heliceModel2));
		helice2.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(turbinaModel1));
		turbina1.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(turbinaModel2));
		turbina2.Draw(shaderProgram, camera);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(chaoModel));
		chao.Draw(shaderProgram, camera);


		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(gravelModel));
		gravel.Draw(shaderProgram, camera);

		

	

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	shaderProgram.Delete();
	shadowMapProgram.Delete();
	shaderSkyBox.Delete();
	glfwDestroyWindow(window);
	
	glfwTerminate();
	return 0;
}