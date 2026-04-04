#version 330 core

in vec2 TexCoord;
layout(location = 0) out vec2 FragUV;

uniform vec2 MouseUV;
uniform float BrushRadius;

void main() {
    FragUV = TexCoord;
}