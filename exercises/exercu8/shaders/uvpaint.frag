#version 330 core

in vec3 WorldPos;
in vec2 TexCoord;
in vec3 WorldNormal;
out vec4 FragColor;

uniform vec3 BrushWorldPos;
uniform vec3 BrushNormal;
uniform float BrushRadius;
uniform float Grow;
uniform float Hair;

void main()
{
    vec2 color = vec2(Grow, Hair);


    if (length(color) > 0){
        FragColor = vec4(color, 0.0, 1.0);
        return;
    }
    
    if (distance(WorldPos, BrushWorldPos) < BrushRadius){
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;    
    }
    FragColor = vec4(0.0);
    return;

    if (Grow > 0.0){
        FragColor = vec4(0.0, Grow, 0.0, 1.0);
        return;
    }
    vec3 toPoint = WorldPos - BrushWorldPos;

    float height = dot(toPoint, BrushNormal);

    // thickness to avoid missing surface
    //if (abs(height) > 0.02)
    //    discard;

    //vec3 projected = toPoint - height * BrushNormal;
    //float dist = length(projected);

    //if (dist > BrushRadius)
    //if (distance(WorldNormal, BrushNormal) + distance(WorldPos, BrushWorldPos) > BrushRadius)
    if (distance(WorldPos, BrushWorldPos) > BrushRadius) // || dot(BrushNormal, WorldNormal) <= 0.0)
    {
        //FragColor = vec4(TexCoord, 0.0, 1.0);
        
        discard;
        //FragColor = vec4(0.0);
    }

 
    else {
        //vec3 color = texture(PaintTexture, TexCoord).rgb + vec3(0.0,0.01,0.0);
        FragColor = vec4(0.01, 0.0, 0.0, 1.0);
        //FragColor = vec4(color, 1.0);
        //FragColor = vec4(TexCoord, 0.0, 1.0);
    }
}