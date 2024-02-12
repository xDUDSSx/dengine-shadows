#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 pvmMatrix;

const int splitCount = 4;

uniform mat4 u_croppedMatrix[splitCount];

uniform int u_splitBegin;
uniform int u_splitEnd; // TODO: Unused remove

out int vSplitIndex;

void main()
{
	int splitIndex = u_splitBegin + gl_InstanceID;
	gl_Position = u_croppedMatrix[splitIndex] * pvmMatrix * vec4(aPos, 1.0);
	vSplitIndex = splitIndex;
}