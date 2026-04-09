#version 330 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec3 VertexTangent;
layout (location = 3) in vec3 VertexBitangent;
layout (location = 4) in vec2 VertexTexCoord;

out vec2 TexCoord;
out vec3 WorldPos;


uniform mat4 WorldViewMatrix;
uniform mat4 WorldViewProjMatrix;
uniform mat4 WorldMatrix;

void main()
{
    WorldPos = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    TexCoord = VertexTexCoord;
    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
}