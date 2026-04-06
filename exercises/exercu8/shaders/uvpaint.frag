#version 330 core

in vec3 WorldPos;
in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 BrushWorldPos;
uniform vec3 BrushNormal;
uniform float BrushRadius;

void main()
{
    //vec3 toPoint = WorldPos - BrushWorldPos;

    //float height = dot(toPoint, BrushNormal);

    // thickness to avoid missing surface
    //if (abs(height) > 0.5)
    //    discard;

    //vec3 projected = toPoint - height * BrushNormal;
    //float dist = length(projected);

    //if (dist > BrushRadius)
    if (distance(WorldPos, BrushWorldPos) > BrushRadius)
    {
        //discard;
        FragColor = vec4(0.0);
    }

 
    else {
        FragColor = vec4(TexCoord, 0.0, 1.0);
    }
}