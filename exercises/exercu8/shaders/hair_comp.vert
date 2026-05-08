
layout(location = 0) in vec3 VertexPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

layout(std430, binding = 0) buffer HairBuffer {
    vec4 instancePositions[];
};

void main()
{
    vec3 offset = instancePositions[gl_InstanceID].xyz;

    vec3 pos = VertexPosition + offset;

    gl_Position = ViewProjMatrix * WorldMatrix * vec4(pos, 1.0);
}