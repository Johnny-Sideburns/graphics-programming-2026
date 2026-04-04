#include "SceneViewerApplication.h"

int main()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    SceneViewerApplication sceneViewerApplication;
    return sceneViewerApplication.Run();
}
