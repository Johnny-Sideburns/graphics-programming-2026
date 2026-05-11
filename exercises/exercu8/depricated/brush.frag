#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D UVTexture;
uniform float BrushRadius;
uniform vec2 MouseUV;


void main()
{
    //vec2 mousePosition = texture(UVTexture, MouseUV).xy;
    vec2 screenUV = gl_FragCoord.xy / vec2(textureSize(UVTexture, 0));

    // UV of model at this pixel
    vec2 uv = texture(UVTexture, screenUV).xy;

    //float dist = distance(screenUV, mousePosition);

    FragColor = vec4(uv, 0.0, 1.0);
}