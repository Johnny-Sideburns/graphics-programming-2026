#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ColorTexture0;



uniform vec4 Color;

void main()
{
	vec4 color0 = texture(ColorTexture0, TexCoord);
	FragColor = Color * color0;
}
