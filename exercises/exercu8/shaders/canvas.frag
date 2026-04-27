#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 BrushWorldPos;
uniform vec3 BrushNormal;
uniform int Mirror;
uniform float TrimLength;

uniform sampler2D BrushTexture;
uniform sampler2D PaintTexture;

void main()
{
    vec3 color0 = texture(BrushTexture, TexCoord).rgb;
    vec3 color1 = texture(PaintTexture, TexCoord).rgb;

    if (color0.r > 0.0){
        FragColor = vec4(0.0, min(color1.g, TrimLength), 0.0, 1.0);
        return;
    }
    if (Mirror > 0)
    {
        color0 = texture(BrushTexture, vec2(TexCoord.x, -TexCoord.y)).rgb;
        if (color0.r > 0.0){
            FragColor = vec4(0.0, min(color1.g, TrimLength), 0.0, 1.0);
            return;
        }
    }

    FragColor = vec4(clamp(color0 + color1, 0.0, 1.0), 1.0);

}
