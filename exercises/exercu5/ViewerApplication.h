#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>

class Texture2DObject;

class ViewerApplication : public Application
{
public:
    ViewerApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeModel();
    void InitializeCamera();
    void InitializeLights();

    void UpdateCamera();

    void RenderGUI();

    void MouseRay();
    void UVDraw(Model model);
    void CreateUVBuffer();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPosition;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;

    // Loaded model
    Model m_model;

    // (todo) 05.X: Add light variables
    glm::vec3 m_ambientColor;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    glm::vec3 m_lightPosition;
    float m_specularExponent;

    // 0.5 self textures
    std::shared_ptr<Texture2DObject> m_millTexture;
    std::shared_ptr<Texture2DObject> m_groundTexture;
    std::shared_ptr<Texture2DObject> m_shadowTexture;

    //mine
    bool m_mbOnePressed;
    glm::vec3 m_mouseRay;
    std::shared_ptr<Material> m_uvMaterial;
    GLuint m_uvFBO;
    GLuint m_materialIDLoc;

    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ShaderProgram> uvLookUpProgram;
};
