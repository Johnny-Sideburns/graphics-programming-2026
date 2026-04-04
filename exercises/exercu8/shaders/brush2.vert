#version 330 core

layout (location = 0) in vec2 VertexPosition; // [-1,1] quad
layout (location = 1) in vec2 VertexTexCoord;

out vec2 FragCoordUV;

void main()
{
    FragCoordUV = VertexTexCoord; // pass to fragment shader
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
}