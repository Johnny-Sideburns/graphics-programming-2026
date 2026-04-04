#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/camera/Camera.h>

#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>

#include <GLFW/glfw3.h>


class GearsApplication : public Application
{
public:
    GearsApplication();


protected:
    void Initialize() override;
    void Update() override;
    void Render() override;

private:
    // Initialize the gear meshes
    void InitializeGeometry();

    // Load, compile and link shaders
    void InitializeShaders();

    // Create a new gear mesh:
    // cogCount: Number of cogs (teeth)
    // innerRadius: Radius of the small circle inside the gear
    // pitchRadius: Radius of the gear, at the distance where it makes contact, in the middle of the cog length
    // addendum: Height of the cogs
    // depth: Size of the cog in Z-coordinate
    void CreateGearMesh(Mesh& mesh, unsigned int cogCount, float innerRadius, float pitchRadius, float addendum, float cogRatio, float depth);

    // Draw a gear mesh with a specific world matrix and color
    void DrawGear(const Mesh& mesh, const glm::mat4& worldMatrix, const Color& color);

    // Helper function to encapsulate loading and compiling a shader
    void LoadAndCompileShader(Shader& shader, const char* path);
    void processInput(GLFWwindow* window);


private:

    // Gear meshes
    Mesh m_largeGear;
    Mesh m_mediumGear;
    Mesh m_smallGear;

    // Shader program
    ShaderProgram m_shaderProgram;

    // Shader uniform locations
    ShaderProgram::Location m_colorUniform;
    ShaderProgram::Location wm_matrixUniform;
    ShaderProgram::Location m_viewProjUniform;

    // (todo) 03.1: Add the world matrix uniform
    glm::mat4 worldMatrixUniform;

    float lastFrameTime;
    float deltaTime;
    float rot = 0.0f;

    // (todo) 03.5: Add the viewProj matrix uniform
    glm::mat4 viewProj;

    // (todo) 03.4: Add the camera
    Camera camera;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    float dimensions;
    //(glm::vec3(0.f,0.f,3.f), glm::vec3(0.f,0.f, -1.f), glm::vec3(0.f,1.f,0.f) );

};
