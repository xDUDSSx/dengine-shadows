#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 pvmMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix; // TODO: (DR) Don't need some of these uniforms

out vec3 vFragPos;

void main()
{
	gl_Position = pvmMatrix * vec4(aPos, 1.0);
	vFragPos = (modelMatrix * vec4(aPos, 1.0)).xyz;
}