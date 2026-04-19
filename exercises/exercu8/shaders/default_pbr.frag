//Inputs
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec3 WorldTangent;
in vec3 WorldBitangent;
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform vec3 Color;
uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D PaintTexture;

uniform vec3 CameraPosition;
void main()
{
    vec4 snot = texture(PaintTexture, TexCoord);
    vec4 snotty = texture(ColorTexture, TexCoord);

    SurfaceData data;

    data.normal = SampleNormalMap(
        NormalTexture,
        TexCoord,
        normalize(WorldNormal),
        normalize(WorldTangent),
        normalize(WorldBitangent)
    );

    // data.albedo = Color * mix(texture(ColorTexture, TexCoord).rgb, snot.rgb, snot.a);
    // data.albedo = Color * texture(ColorTexture, TexCoord).rgb;


    vec3 pale = vec3(1.0, 0.859, 0.675);
    vec3 tan  = vec3(0.847, 0.686, 0.475);
    vec3 dark = vec3(0.553, 0.333, 0.141);

    //data.albedo = Color * dark;
    //data.albedo = Color * mix(tan, snot.rgb, snot.a);
    data.albedo = Color * mix(tan, dark - vec3(0.02,0.02,0.02), clamp(snotty.r * snot.g * 5, 0.0 , 2.0));

    // vec3 arm = texture(SpecularTexture, TexCoord).rgb;
    // data.ambientOcclusion = arm.x;
    // data.roughness        = arm.y;
    // data.metalness        = arm.z;

    data.ambientOcclusion = 0.5;
    data.roughness        = 0.8;
    data.metalness        = 0.0;

    vec3 position = WorldPosition;
    vec3 viewDir  = GetDirection(position, CameraPosition);

    vec3 color = ComputeLighting(position, data, viewDir, true);

    FragColor = vec4(color.rgb, 1.0);
    
    //FragColor = vec4(snot);
    //FragColor = vec4(data.albedo, 1);
}