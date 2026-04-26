#include "Painter.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>



#include "iostream"

//for scroll there needs to be a static value it's only used here so it'll do
namespace Input
{
    float scrollY = 10.0f;
}

Painter::Painter(Window& window, Renderer& renderer, std::shared_ptr<Texture2DObject> target)
{
    int width, height;
    window.GetDimensions(width, height);
    m_paintRenderPass = std::make_unique<PaintRenderPass>(width, height, renderer, target);

    m_paint = m_paintRenderPass->GetPaintPtr();
    m_brushRadius = m_paintRenderPass->GetBrushRadius();
    m_mousePosition = m_paintRenderPass->GetMousePosPtr();
    m_grow = m_paintRenderPass->GetGrowFloat();

    //add the paint render pass the the renderer
    renderer.AddRenderPass(std::move(m_paintRenderPass));

    //initialise scroll callback for incorporating scroll
    glfwSetScrollCallback(window.GetInternalWindow(), scroll_callback);


}

void Painter::Update(const Window& window, float deltaTime)
{
    UpdateBrushScale();

    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        std::cout << "fps: " << 1.f/deltaTime << std::endl;
        Paint(window);
    }
    //press p to grow beard
    if (window.IsKeyPressed(GLFW_KEY_P)) {

        *m_grow = 0.003;
        *m_paint = true;
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
    *m_mousePosition = glm::vec2(pos[0], height - pos[1]);

    *m_paint = true;
}