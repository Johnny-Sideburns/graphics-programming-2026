#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform float SpecularReflection;
uniform float SpecularExponent;

uniform sampler2D ColorTexture0;


uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;
uniform vec4 Color;

vec3 getDiffuseReflection(vec3 color, vec3 l, vec3 n)
{
	vec3 result = LightColor * DiffuseReflection * color * dot(l, n);
	return result;
}

vec3 getAmbientReflection(vec3 color)
{
	return AmbientColor * AmbientReflection * color;
}

vec3 getSpecularReflection(vec3 l, vec3 n, vec3 view)
{
	return LightColor * SpecularReflection * pow(clamp(dot(n, normalize(l + view)), 0.0f, 1.0f), SpecularExponent);
}

vec3 getBlinnPhongReflection(vec3 color, vec3 l, vec3 n, vec3 view)
{
	return getAmbientReflection(color) + getDiffuseReflection(color, l, n) + getSpecularReflection(l, n, view);
}

void main()
{
	vec4 color0 = texture(ColorTexture0, TexCoord);
	
	vec3 lightVector = normalize(LightPosition - WorldPosition);
	vec3 cameraVector = normalize(CameraPosition - WorldPosition) * 0.5f;

	vec3 reflectColor = getBlinnPhongReflection(vec3(color0.x,color0.y,color0.z), lightVector, normalize(WorldNormal), cameraVector);
	
	vec4 color = vec4(reflectColor.x,reflectColor.y,reflectColor.z, color0.w);
	FragColor = Color * color;
}
