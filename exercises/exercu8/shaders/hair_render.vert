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
uniform vec3 CameraPosition;
uniform int Segments;

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

    int segID = gl_VertexID / 2;
    int side  = gl_VertexID % 2;

    float t = float(segID) / float(Segments);

    vec3 center = bezier(s.p0, s.p1, s.p2, s.p3, t);
    vec3 next   = bezier(s.p0, s.p1, s.p2, s.p3, t + 0.01);

    vec3 dir = normalize(next - center);
    
    vec3 viewDir = normalize(center - CameraPosition);
    
    vec3 c = cross(viewDir, dir);

    //if viewdir and dir are too close set to 1
    if (length(c) < 1e-5)
        c = vec3(1,0,0);

    vec3 right = normalize(c);

    float offset = (side == 0 ? -1.0 : 1.0) * s.width;

    vec3 world = center + right * offset;

    gl_Position = ViewProjMatrix * vec4(world, 1.0);
}