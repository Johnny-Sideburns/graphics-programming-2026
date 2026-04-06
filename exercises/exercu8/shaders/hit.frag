#version 330 core

in vec3 Normal;

layout(location = 0) out vec4 FragColor;
//layout(location = 1) out uint OutID;

//uniform uint ModelId;

void main()
{
    //OutID = ModelId;
    FragColor = vec4(normalize(Normal), 1.0);
}