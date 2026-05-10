#version 330

layout(location=0) in vec2 corner; // (-1, +1) ribbon width

struct Strand
{
    vec3 p0, p1, p2, p3;
    vec2 uv;
    float width;
    float seed;
    float grow;
};

layout(std430, binding=0) buffer StrandBuffer
{
    Strand strands[];
};

uniform mat4 ViewProjMatrix;
uniform int segments;

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
    int strandID = gl_InstanceID;

    Strand s = strands[strandID];

    float t = float(gl_VertexID) / float(segments);

    vec3 center = bezier(s.p0, s.p1, s.p2, s.p3, t);

    vec3 next = bezier(s.p0, s.p1, s.p2, s.p3, t + 0.01);
    vec3 dir = normalize(next - center);

    vec3 viewDir = normalize(-center); // replace with camera uniform
    vec3 side = normalize(cross(viewDir, dir)) * s.width;

    vec3 world = center + side * corner.x;

    gl_Position = ViewProjMatrix * vec4(world, 1.0);
}