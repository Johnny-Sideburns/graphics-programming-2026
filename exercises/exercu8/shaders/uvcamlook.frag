#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;

out vec2 FragColor;

uniform vec3 BrushWorldPos;
uniform float BrushRadius;

void main()
{
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor = texture(PaintTexture, TexCoord);
    if(distance(WorldPos, BrushWorldPos) < BrushRadius){
        FragColor = TexCoord;
    }
    else {
        //discard;
        FragColor = TexCoord;

    }
}