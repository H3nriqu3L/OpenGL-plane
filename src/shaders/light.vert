#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model_light;
uniform mat4 camMatrix;

void main()
{
	gl_Position = camMatrix * model_light * vec4(aPos,1.0f);
}