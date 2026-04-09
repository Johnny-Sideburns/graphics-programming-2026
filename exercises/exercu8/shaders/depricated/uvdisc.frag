#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 WorldNormal;

out vec2 FragColor;

uniform vec3 MouseWP;
uniform vec3 BrushNormal;
uniform float BrushRadius;


void main()
{
    float discThick = 0.02;

    // Vector from brush center to this fragment
    vec3 toPoint = WorldPos - MouseWP;

    // Distance along the normal (height off the plane)
    float height = dot(toPoint, BrushNormal);

    // Reject if too far off the surface plane
    if (abs(height) > discThick)
    {
        FragColor = TexCoord;

        //discard;
        //FragColor = BrushNormal.xy;
        return;
    }

    // Project onto the brush plane
    vec3 projected = toPoint - height * BrushNormal;

    // Radial distance inside the plane
    float dist = length(projected);

    if (dist < BrushRadius)
    {
        FragColor = TexCoord;
        //discard;
    }
    else
    {
        //FragColor = BrushNormal.xy;
        FragColor = TexCoord;
        
        //discard;
    }
}
