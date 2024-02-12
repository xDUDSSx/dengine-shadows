#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 15) out;

const int splitCount = 4;

uniform mat4 u_croppedMatrix[splitCount];

uniform int u_splitBegin;
uniform int u_splitEnd;

void main()
{
    for (int i = u_splitBegin; i <= u_splitEnd; i++)
    {
        gl_Layer = i;
        for(int v = 0; v < 3; v++)
        {
            gl_Position = u_croppedMatrix[i] * gl_in[v].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}