#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 pvmMatrix;

void main()
{
	gl_Position = pvmMatrix * vec4(aPos, 1.0);
}