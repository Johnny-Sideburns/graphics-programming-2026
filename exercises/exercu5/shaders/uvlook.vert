#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;


flat out int vMaterialID;
uniform int uMaterialID;
out vec2 TexCoord;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main()
{
    TexCoord = VertexTexCoord;
    vMaterialID = uMaterialID;
    vec3 worldPos = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    gl_Position = ViewProjMatrix * vec4(worldPos, 1.0);
}