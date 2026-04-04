#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;
out vec4 FragColor;

uniform sampler2D UVTexture;
uniform sampler2D PaintTexture;
uniform vec3 BrushWorldPos;
uniform vec2 MouseUV;
uniform float BrushRadius;

void main()
{
    vec2 uv = texture(UVTexture, TexCoord).xy;
    //vec2 uv = texture(UVTexture, gl_FragCoord.xy / textureSize(UVTexture, 0)).xy;

    float dist = distance(WorldPos, BrushWorldPos);

    if (dist < BrushRadius)
    {
        // Paint this fragment
        //FragColor = vec4(uv,0.0,1.0);
        FragColor = vec4(TexCoord, 0.0, 1.0);
    }
    else
    {
        //FragColor = vec4(TexCoord,1.0, 1.0);
        //FragColor = texture(PaintTexture, TexCoord);
        //discard;
    }

}