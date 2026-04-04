#version 330 core

in vec2 TexCoord;
flat in int vMaterialID;

layout(location = 0) out vec2 outUV;
layout(location = 1) out int outMaterialID;

void main()
{
    outUV = TexCoord;
    outMaterialID = vMaterialID;
}
