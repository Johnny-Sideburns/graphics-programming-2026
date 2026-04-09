#version 330 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 4) in vec2 VertexTexCoord;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 WorldNormal;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform vec3 BrushPos;
uniform vec3 BrushNormal;

void main()
{
    // Transform vertex to world space
    vec3 worldPos = (WorldMatrix * vec4(VertexPosition,1.0)).xyz;

    // Compute brush-aligned view matrix (like a mini camera)
    vec3 forward = normalize(-BrushNormal); // looking toward brush position
    vec3 right = normalize(cross(vec3(0.0,1.0,0.0), forward));
    vec3 up = cross(forward, right);

    mat4 BrushView = mat4(
        vec4(right,0.0),
        vec4(up,0.0),
        vec4(forward,0.0),
        vec4(0.0,0.0,0.0,1.0)
    );

    vec3 BrushCameraPos = BrushPos + BrushNormal * 2.0;

    // translate world positions into brush space
    vec4 brushSpacePos = BrushView * vec4(worldPos - BrushCameraPos, 1.0);

    // now project into NDC using your existing projection
    gl_Position = ViewProjMatrix * brushSpacePos;
    TexCoord = VertexTexCoord;
    WorldPos = worldPos;
    WorldNormal = normalize(mat3(WorldMatrix) * VertexNormal);

}