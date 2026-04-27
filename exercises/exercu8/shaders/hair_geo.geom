
layout(triangles) in;
layout(triangle_strip, max_vertices = 128) out;


in vec3 WorldPosition[];
in vec3 WorldNormal[];
in vec3 WorldTangent[];
in vec3 WorldBitangent[];
in vec2 TexCoord[];

out vec3 g_WorldPosition;
out vec3 g_WorldNormal;
out vec3 g_WorldTangent;
out vec3 g_WorldBitangent;
out vec2 g_TexCoord;
out float g_IsHair;

uniform mat4 ViewProjMatrix;

uniform sampler2D PaintTexture;
uniform sampler2D ColorTexture;


// seudo deterministic random coutesy of:
// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com
// https://thebookofshaders.com/10/
float random (vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// random point inside current triangle
vec3 getStrandPos(int s, out vec2 uv)
{
    float r1 = random(WorldPosition[0].xy + float(s));
    float r2 = random(WorldPosition[1].xy + float(s)*2.0);

    float u = r1;
    float v = r2 * (1.0 - u);
    float w = 1.0 - u - v;

    uv =
        u * TexCoord[0] +
        v * TexCoord[1] +
        w * TexCoord[2];

    return
        u * WorldPosition[0] +
        v * WorldPosition[1] +
        w * WorldPosition[2];
}

// interpolated normal (stable)
vec3 getNormal(vec2 uv)
{
    // barycentric again but reuse logic if you want
    // here just average (cheap + stable baseline)
    return normalize(
        WorldNormal[0] +
        WorldNormal[1] +
        WorldNormal[2]);
}

// cubic bezier
vec3 bezier(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
    float it = 1.0 - t;
    return it*it*it*a +
           3.0*it*it*t*b +
           3.0*it*t*t*c +
           t*t*t*d;
}

void main()
{
    for (int i = 0; i < 3; i++)
    {
                
        g_WorldPosition = WorldPosition[i];
        g_WorldNormal = WorldNormal[i];
        g_WorldTangent = WorldTangent[i];
        g_WorldBitangent = WorldBitangent[i];
        g_TexCoord = TexCoord[i];
        g_IsHair = 0;
        gl_Position = gl_in[i].gl_Position;

        EmitVertex();
    }
    EndPrimitive();

    float value = texture(ColorTexture, TexCoord[0]).r + texture(ColorTexture, TexCoord[1]).r + texture(ColorTexture, TexCoord[2]).r;
    float grow = texture(PaintTexture, TexCoord[0]).g + texture(PaintTexture, TexCoord[1]).g + texture(PaintTexture, TexCoord[2]).g;
    if (value * grow > 0.0)
    {
        // --- 2. strands ---
    int strandCount = 1;
    int segments = 6;
    float width = 0.0003;

    for (int s = 0; s < strandCount; s++)
    {
        vec2 uv;
        vec3 pos = getStrandPos(s, uv);

        float grow = texture(PaintTexture, uv).g;
        float mask = texture(ColorTexture, uv).r;

        if (grow * mask <= 0.0)
            continue;

        vec3 normal = getNormal(uv);

        // random variation
        float r = random(pos.xy + float(s));

        float height = mix(0.15, 0.2, r);

        vec3 bendDir = normalize(cross(normal, vec3(0,1,0)));

        // --- bezier control points ---
     
        vec3 gravity = vec3(0,-3,0);

        vec3 p0 = pos;
        vec3 p1 = pos + normal * (height * 0.3);
        vec3 p2 = pos + normalize(normal + gravity * 0.5) * (height * 0.6) + bendDir * 0.05 * r;
        vec3 p3 = pos + normalize(normal + gravity) * height;

        // --- build strand ---
        for (int i = 0; i <= segments; i++)
        {
            float t = clamp (float(i) / float(segments), 0.0 , grow * mask);

            vec3 center = bezier(p0, p1, p2, p3, t);

            float dt = 0.01;
            vec3 next = bezier(p0, p1, p2, p3, t + dt);
            vec3 dir = normalize(next - center);

            vec3 up = abs(dir.y) > 0.9 ? vec3(1,0,0) : vec3(0,1,0);
            vec3 side = normalize(cross(dir, normal)) * width;

            vec3 left  = center - side;
            vec3 right = center + side;

            vec3 strandNormal = normalize(cross(side, dir));
            g_WorldNormal = strandNormal;

            
            g_TexCoord = uv;

            g_WorldPosition = left;
            gl_Position = ViewProjMatrix * vec4(left, 1.0);
            EmitVertex();

            g_WorldPosition = right;
            g_IsHair = 1;
            gl_Position = ViewProjMatrix * vec4(right, 1.0);
            EmitVertex();
        }

        EndPrimitive();
        }
    };

}