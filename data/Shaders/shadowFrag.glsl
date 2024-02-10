#version 330 core

in vec3 vFragPos;

uniform float u_zFar;
//uniform vec3 u_lightPos;
uniform vec3 u_sunPos;
uniform vec3 u_sunDir;

float distancePointToPlane(vec3 point, vec3 planePoint, vec3 planeNormal)
{
    return dot(planeNormal, point - planePoint);
}

void main()
{
    float lightDist = distancePointToPlane(vFragPos, u_sunPos, u_sunDir);
//    float lightDist = length(vFragPos - u_lightPos);
    lightDist /= u_zFar; // Convert to [0, 1]
    gl_FragDepth = lightDist;
}