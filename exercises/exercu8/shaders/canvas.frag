#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 BrushWorldPos;
uniform vec3 BrushNormal;

uniform sampler2D BrushTexture;
uniform sampler2D PaintTexture;

void main()
{
    vec3 color0 = texture(BrushTexture, TexCoord).rgb;
    vec3 color1 = texture(PaintTexture, TexCoord).rgb;
    if (color0.r > 0.0){
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    FragColor = vec4(clamp(color0 + color1, 0.0, 1.0), 1.0);
}
