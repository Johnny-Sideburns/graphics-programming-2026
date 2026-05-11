#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D UVTexture;
uniform float BrushRadius;
uniform vec2 MouseUV;


void main()
{
    vec2 mousePosition = texture(UVTexture, MouseUV).xy;
    //vec2 screenUV = gl_FragCoord.xy / vec2(textureSize(UVTexture, 0));
    vec2 screenUV = texture(UVTexture, TexCoord).xy;

    // UV of model at this pixel
    vec2 uv = texture(UVTexture, screenUV).xy;

    float dist = distance(screenUV, mousePosition);

    if (dist < BrushRadius)
    {
        FragColor = vec4(0.5, 0.5, 0.0, 1.0);
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        //discard;
    }
}