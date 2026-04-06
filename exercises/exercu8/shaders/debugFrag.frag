#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D PaintTexture;


void main()
{
    FragColor = vec4(TexCoord, 0.0, 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor = texture(PaintTexture, TexCoord);
}
