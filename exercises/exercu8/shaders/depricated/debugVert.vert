#version 330 core

layout (location = 0) in vec3 VertexPosition;
//layout (location = 1) in vec3 VertexNormal;
layout (location = 1) in vec2 VertexTexCoord;

//uniform mat4 WorldViewMatrix;
//uniform mat4 WorldViewProjMatrix;

out vec2 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}