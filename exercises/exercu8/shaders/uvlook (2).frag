#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;
layout(location = 0) out vec2 FragColor;

uniform vec3 BrushWorldPos;
uniform float BrushRadius;

void main() {
    float dist = distance(WorldPos, BrushWorldPos);
    if (dist < BrushRadius){
        FragColor = vec2(TexCoord);
    }
    else {
        discard;
    }
}