#include "Painter.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>



#include "iostream"

namespace Input
{
    float scrollY = 10.0f;
}

Painter::Painter(){}
Painter::Painter(Window& window, Renderer& renderer)
{
    //InitializeShaderProgram(renderer);
    int width, height;
    window.GetDimensions(width, height);
    m_paintRenderPass = std::make_unique<PaintRenderPass>(width, height, renderer);
    m_depthTexture = m_paintRenderPass->GetDepthTexture();
    m_paintTexture = m_paintRenderPass->GetPaintTexture();
    m_paint = m_paintRenderPass->GetPaintPtr();
    m_brushRadius = m_paintRenderPass->GetBrushRadius();
    m_mousePosition = m_paintRenderPass->GetMousePosPtr();
    renderer.AddRenderPass(std::move(m_paintRenderPass));

    glfwSetScrollCallback(window.GetInternalWindow(), scroll_callback);


}

void Painter::Update(const Window& window, float deltaTime)
{
    UpdateBrushScale();
    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        std::cout << "fps: " << 1.f/deltaTime << std::endl;
        Paint(window);
    }
    
}

void Painter::UpdateBrushScale()
{
    *m_brushRadius = Input::scrollY/200.f;
}

void Painter::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (Input::scrollY + (float)yoffset >= 200.f) return;
    if (Input::scrollY + (float)yoffset <= 0.0f) return;
    Input::scrollY += (float)yoffset;
}

void Painter::Paint(const Window& window)
{
    int width, height;
    window.GetDimensions(width, height);

    glm::vec2 pos = window.GetMousePosition();
    //*m_mousePosition = glm::vec2((pos[0] + 1) / 2, (pos[1] + 1) / 2);
    *m_mousePosition = glm::vec2(pos[0], height - pos[1]);

    *m_paint = true;
}