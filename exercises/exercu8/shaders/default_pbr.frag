//Inputs
in vec3 g_WorldPosition;
in vec3 g_WorldNormal;
in vec3 g_WorldTangent;
in vec3 g_WorldBitangent;
in vec2 g_TexCoord;
in float g_IsHair;

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
    vec4 snot = texture(PaintTexture, g_TexCoord);
    vec4 snotty = texture(ColorTexture, g_TexCoord);
    SurfaceData data;

    data.normal = SampleNormalMap(
        NormalTexture,
        g_TexCoord,
        normalize(g_WorldNormal),
        normalize(g_WorldTangent),
        normalize(g_WorldBitangent)
    );

    // data.albedo = Color * mix(texture(ColorTexture, g_TexCoord).rgb, snot.rgb, snot.a);
    // data.albedo = Color * texture(ColorTexture, g_TexCoord).rgb;


    vec3 pale = vec3(1.0, 0.859, 0.675);
    vec3 tan  = vec3(0.847, 0.686, 0.475);
    vec3 dark = vec3(0.553, 0.333, 0.141);

    //data.albedo = Color * dark;
    //data.albedo = Color * mix(tan, snot.rgb, snot.a);
    data.albedo = Color * mix(tan, dark - vec3(0.02,0.02,0.02), clamp(snotty.r * snot.g * 5, 0.0 , 2.0));

    if (g_IsHair > 0.0){

    }
    else {
        data.albedo = Color * pale;
    
    }

    // vec3 arm = texture(SpecularTexture, g_TexCoord).rgb;
    // data.ambientOcclusion = arm.x;
    // data.roughness        = arm.y;
    // data.metalness        = arm.z;

    data.ambientOcclusion = 0.5;
    data.roughness        = 0.8;
    data.metalness        = 0.0;

    vec3 position = g_WorldPosition;
    vec3 viewDir  = GetDirection(position, CameraPosition);

    vec3 color = ComputeLighting(position, data, viewDir, true);

    FragColor = vec4(color.rgb, 1.0);
    
    //FragColor = vec4(snot);
    //FragColor = vec4(data.albedo, 1);
}