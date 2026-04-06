#include "Painter.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>



#include "iostream"

Painter::Painter(){}
Painter::Painter(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target)
{
    //InitializeShaderProgram(renderer);
    m_paintRenderPass = std::make_unique<PaintRenderPass>(width, height, renderer, target);
    //m_renderer = renderer;
    m_depthTexture = m_paintRenderPass->GetDepthTexture();
    m_paintTexture = m_paintRenderPass->GetPaintTexture();
    m_shaderProgramPtr = m_paintRenderPass->GetShaderProgram();
    m_paint = m_paintRenderPass->GetPaintPtr();
    m_mousePosition = m_paintRenderPass->GetMousePosPtr();
    renderer.AddRenderPass(std::move(m_paintRenderPass));
    m_targetTexture = target;
}

void Painter::Update(const Window& window, float deltaTime)
{
    if (m_delay > 0.0f) {
        m_delay -= deltaTime;
        return;
    }
    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        std::cout << "fps: " << 1.f/deltaTime << std::endl;
        Paint(window);
    }
}

void Painter::Paint(const Window& window)
{
    int width, height;
    window.GetDimensions(width, height);

    glm::vec2 pos = window.GetMousePosition();
    //*m_mousePosition = glm::vec2((pos[0] + 1) / 2, (pos[1] + 1) / 2);
    *m_mousePosition = glm::vec2(pos[0], height - pos[1]);

    *m_paint = true;
    m_delay = 0.033;
}

/*
void Painter::InitializeShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/uvlook.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/uvlook.frag");
    m_shaderProgramPtr = std::make_shared<ShaderProgram>();
    m_shaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldViewMatrixLocation = m_shaderProgramPtr->GetUniformLocation("WorldViewMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    ShaderProgram::Location paintTextureLocation = m_shaderProgramPtr->GetUniformLocation("PaintTexture");


    renderer.RegisterShaderProgram(m_shaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
        },
        nullptr
    );
}



Painter::Painter(){}
Painter::Painter(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target)
{
    //InitializeShaderProgram(renderer);
    m_paintRenderPass = std::make_unique<PaintRenderPass>(width, height, renderer, target);
    //m_renderer = renderer;
    m_depthTexture = m_paintRenderPass->GetDepthTexture();
    m_paintTexture = m_paintRenderPass->GetPaintTexture();
    m_shaderProgramPtr = m_paintRenderPass->GetShaderProgram();
    m_paint = m_paintRenderPass->GetPaintPtr();
    m_mousePosition = m_paintRenderPass->GetMousePosPtr();
    renderer.AddRenderPass(std::move(m_paintRenderPass));
    m_targetTexture = target;
}

void Painter::Update(const Window& window, float deltaTime)
{
    if (m_delay > 0.0f) {
        m_delay -= deltaTime;
        return;
    }
    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        std::cout << "fps: " << 1.f/deltaTime << std::endl;
        Paint(window);
    }
}

void Painter::Paint(const Window& window)
{
    int width, height;
    window.GetDimensions(width, height);

    //glm::vec2 pos = window.GetMousePosition(true);
    //*m_mousePosition = glm::vec2((pos[0] + 1) / 2, (pos[1] + 1) / 2);
    *m_mousePosition = window.GetMousePosition();

    *m_paint = true;
    m_delay = 0.033;
}



*/