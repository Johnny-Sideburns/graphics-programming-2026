#version 330 core

in vec2 FragCoordUV;
out vec4 FragColor;

uniform sampler2D UVTexture;   // RG: UVs of pixels under brush
uniform sampler2D PaintTexture; // Current paint texture
//uniform vec4 BrushColor;       // Color to paint

void main()
{
    vec2 uv = texture(UVTexture, FragCoordUV).rg;

    // If mask is empty, discard
    if(uv == vec2(0.0)) 
    {
        discard;
    }
    else {
    // Transform UV to paint texture coordinates
    vec2 paintTexCoords = uv; // assuming normalized [0,1]

    // Read current paint color
    vec4 currentColor = texture(PaintTexture, paintTexCoords);

    // Blend new color
    //vec4 finalColor = mix(currentColor, BrushColor, 1.0); // fully overwrite or adjust alpha

    FragColor = vec4(uv,0.0,1.0);//finalColor;
    }
}