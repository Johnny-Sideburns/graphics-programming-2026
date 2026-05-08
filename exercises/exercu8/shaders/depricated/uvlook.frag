#version 330 core

in vec2 TexCoord;
layout(location = 0) out vec2 FragUV;

uniform vec2 MouseUV;
uniform float BrushRadius;

void main() {
    vec2 screenUV = gl_FragCoord.xy / 1024;
    float dist = distance(screenUV, MouseUV);

    if (dist < 0.05)
    {
        FragUV = TexCoord;
        //FragUV = MouseUV;
        //FragUV = vec2(dist,0.0);
        //FragUV = vec2(0.0,0.0);

    }
    else
    {
        //FragUV = TexCoord;
        //FragUV = MouseUV;
        //FragUV = vec2(dist,0.0);
        //FragUV = vec2(0.0,0.0);
        discard;

    }
}
