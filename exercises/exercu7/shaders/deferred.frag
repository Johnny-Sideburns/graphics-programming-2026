//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D DepthTexture;
uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D OthersTexture;
uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;

void main()
{	vec3 albedo = texture(AlbedoTexture, TexCoord).rgb;
	vec3 normal = GetImplicitNormal(texture(NormalTexture, TexCoord).xy);
	vec3 position = ReconstructViewPosition(DepthTexture, TexCoord, InvProjMatrix);
	vec3 viewDir = normalize((InvViewMatrix * vec4(-position, 0.f) ).xyz);
	position = (InvViewMatrix * vec4(position + vec3(0.0f,0.0f,-0.5f), 1.f)).xyz;

	vec4 others = texture(OthersTexture, TexCoord).xyzw;
	SurfaceData data;
	data.normal = normalize((InvViewMatrix * vec4(normal, 0.f)).xyz);
	data.reflectionColor = albedo;
	data.ambientReflectance = others.x;
	data.diffuseReflectance = others.y;
	data.specularReflectance = others.z;
	data.specularExponent = (1.f / others.w) -1.f;

	vec3 color = ComputeLighting(position, data, viewDir, true);

	FragColor = vec4(color,1.f);
}
