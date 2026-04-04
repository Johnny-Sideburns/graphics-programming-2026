#include "ViewerApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>

#include <iostream>
#include <filesystem>


ViewerApplication::ViewerApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(0, 30, 30)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
{
}

void ViewerApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeModel();
    InitializeCamera();
    InitializeLights();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void ViewerApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void ViewerApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    UVDraw(m_model);
    m_model.Draw();
    RenderGUI();
}

void ViewerApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void ViewerApplication::InitializeModel()
{   
    // (todo 05.0) (live coding)
    printf("During the live coding, we will look into loading and configuring a 3D model\n");
    CreateUVBuffer();

    /*
    std::filesystem::path dir = ".";
    std::cout << std::filesystem::current_path() << '\n';
    std::cout << std::filesystem::absolute(dir) << '\n';
    std::cout << std::filesystem::exists("shaders/shader.vert") << '\n';

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        std::cout << entry.path().filename() << '\n';
    }
    // Load and build shader
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/unlit.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/unlit.frag");
    */
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/blinn-phong.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/blinn-phong.frag");
    shaderProgram = std::make_shared<ShaderProgram>();
    shaderProgram->Build(vertexShader, fragmentShader);

    //uv lookup trying
    Shader UVvertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/uvlook.vert");
    Shader UVfragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/uvlook.frag");
    uvLookUpProgram = std::make_shared<ShaderProgram>();
    uvLookUpProgram->Build(UVvertexShader, UVfragmentShader);


    // Filter out uniforms that are not material properties
    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");
    filteredUniforms.insert("AmbientColor");
    filteredUniforms.insert("LightColor");
    filteredUniforms.insert("LightPosition");
    filteredUniforms.insert("CameraPosition");

    //and for uv shader
    ShaderUniformCollection::NameSet UVfilteredUniforms;
    UVfilteredUniforms.insert("WorldMatrix");
    UVfilteredUniforms.insert("ViewProjMatrix");
    m_uvMaterial = std::make_shared<Material>(uvLookUpProgram, UVfilteredUniforms);

    
    // Create reference material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgram, filteredUniforms);
    material->SetUniformValue("Color", glm::vec4(1.0f));
    material->SetUniformValue("AmbientReflection", 1.0f);
    material->SetUniformValue("DiffuseReflection", 1.0f);
    material->SetUniformValue("SpecularReflection", 1.0f);
    material->SetUniformValue("SpecularExponent", 100.f);

    /*
    ShaderProgram::Location ambientReflectionLocation = shaderProgram->GetUniformLocation("AmbientReflection");
    std::cout << ambientReflectionLocation << std::endl;
    */

    m_ambientColor = glm::vec3(0.27f, 0.26f, 0.25f);
    m_lightColor = glm::vec3(1.0f);
    m_lightIntensity = 1.f;
    m_lightPosition = glm::vec3(-20.f, 20.f, 10.f);
    m_specularExponent = 100.f;


    // Setup function
    ShaderProgram::Location worldMatrixLocation = shaderProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = shaderProgram->GetUniformLocation("ViewProjMatrix");
    ShaderProgram::Location ambientColorLocation = shaderProgram->GetUniformLocation("AmbientColor");
    ShaderProgram::Location lightColorLocation = shaderProgram->GetUniformLocation("LightColor");
    ShaderProgram::Location lightPositionLocation = shaderProgram->GetUniformLocation("LightPosition");
    ShaderProgram::Location cameraPositionLocation = shaderProgram->GetUniformLocation("CameraPosition");
    ShaderProgram::Location specularExponentLocation = shaderProgram->GetUniformLocation("SpecularExponent");

    material->SetShaderSetupFunction([=](ShaderProgram& shaderProgram)
        {
            shaderProgram.SetUniform(worldMatrixLocation, glm::scale(glm::vec3(0.1f)));
            shaderProgram.SetUniform(viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());

            // (todo) 05.X: Set camera and light uniforms
            shaderProgram.SetUniform(ambientColorLocation, m_ambientColor);
            shaderProgram.SetUniform(lightColorLocation, m_lightColor * m_lightIntensity);
            shaderProgram.SetUniform(lightPositionLocation, m_lightPosition);
            shaderProgram.SetUniform(cameraPositionLocation, m_cameraPosition);
            shaderProgram.SetUniform(specularExponentLocation, m_specularExponent);


        });

    // Setup function
    ShaderProgram::Location UVworldMatrixLocation = uvLookUpProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location UVviewProjMatrixLocation = uvLookUpProgram->GetUniformLocation("ViewProjMatrix");
    m_materialIDLoc = uvLookUpProgram->GetUniformLocation("uMaterialID");
    std::cout << m_materialIDLoc << std::endl;

    m_uvMaterial->SetShaderSetupFunction([=](ShaderProgram& uvLookUpProgram)
        {
            uvLookUpProgram.SetUniform(UVworldMatrixLocation, glm::scale(glm::vec3(0.1f)));
            uvLookUpProgram.SetUniform(UVviewProjMatrixLocation, m_camera.GetViewProjectionMatrix());
            uvLookUpProgram.SetUniform(m_materialIDLoc, 1);
        });

    // Configure loader
    ModelLoader loader(material);
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");
    loader.SetCreateMaterials(true);

    // Load model
    m_model = loader.Load("models/mill/Mill.obj");

    // (todo) 05.1: Load and set textures
    std::cout << m_model.GetMaterialCount() <<std::endl;
    
    std::shared_ptr<Material> material_shadow = std::make_shared<Material>(m_model.GetMaterial(0));
    std::shared_ptr<Material> material_ground = std::make_shared<Material>(m_model.GetMaterial(1));
    std::shared_ptr<Material> material_mill = std::make_shared<Material>(m_model.GetMaterial(2));
    
    Texture2DLoader textureLoader(TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);

    textureLoader.SetFlipVertical(true);

    m_shadowTexture = std::make_shared<Texture2DObject>(textureLoader.Load("models/mill/Ground_shadow.jpg"));
    m_groundTexture = std::make_shared<Texture2DObject>(textureLoader.Load("models/mill/Ground_color.jpg"));
    m_millTexture = std::make_shared<Texture2DObject>(textureLoader.Load("models/mill/MillCat_color.jpg"));

    /*
    */

    material_shadow->SetUniformValue("ColorTexture0", m_shadowTexture);
    material_shadow->SetUniformValue("AmbientReflection", 0.3f);
    material_shadow->SetUniformValue("SpecularReflection", 0.f);

    material_ground->SetUniformValue("ColorTexture0", m_groundTexture);
    material_ground->SetUniformValue("AmbientReflection", 1.f);
    material_ground->SetUniformValue("SpecularReflection", 0.2f);

    material_mill->SetUniformValue("ColorTexture0", m_millTexture);
    material_mill->SetUniformValue("AmbientReflection", 1.f);
    material_mill->SetUniformValue("SpecularReflection", 0.6f);

    
    m_model.SetMaterial(0, material_shadow);
    m_model.SetMaterial(1, material_ground);
    m_model.SetMaterial(2, material_mill);
    

}

void ViewerApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void ViewerApplication::InitializeLights()
{
    // (todo) 05.X: Initialize light variables

}

void ViewerApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // (todo) 05.4: Add debug controls for light properties
   
    ImGui::DragFloat("Light Intensity", &m_lightIntensity, 0.01f);
    ImGui::ColorEdit3("Light Color", &m_lightColor.x);
    ImGui::DragFloat3("Light Position", &m_lightPosition.x);
    ImGui::NewLine();
    ImGui::DragFloat("Specular Exponent", &m_specularExponent);
    ImGui::NewLine();
    ImGui::ColorEdit3("Ambient Color", &m_ambientColor.x);
    ImGui::NewLine();
    ImGui::DragFloat3("Mouse Ray", &m_mouseRay.x);

    
    m_imGui.EndFrame();
}

void ViewerApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    if (m_mbOnePressed) //&& window.IsMouseButtonReleased(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
    {   std::cout << "clicking" << std::endl;
        m_mbOnePressed = false;
        
        
        
    }
    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        m_mbOnePressed = true;
        MouseRay();
    }

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
   {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0,1,0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

   // Update view matrix
   m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
   

}
void ViewerApplication::MouseRay()
{
    uvLookUpProgram->Use();
    Window& window = GetMainWindow();
    int width, height;
    window.GetDimensions(width, height);
    glm::vec2 mp = window.GetMousePosition();

    float uv[2];
    int matID;
    glBindFramebuffer(GL_FRAMEBUFFER, m_uvFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(mp.x, height - mp.y, 1, 1, GL_RG, GL_FLOAT, uv);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(mp.x, height - mp.y, 1, 1, GL_RED_INTEGER, GL_INT, &matID);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout << "U: " << uv[0] << ", V: " << uv[1] << std::endl;
    std::cout << matID << std::endl;
    ShaderProgram::Location texLoc = m_model.GetMaterial(matID).GetUniformLocation("ColorTexture0");
    std::shared_ptr<Texture2DObject> t; 
    shaderProgram->Use();
    //m_model.GetMaterial(matID).GetUniformValue(texLoc, t);
    GLuint textureID = texLoc;
    int texWidth, texHeight;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Map UV [0,1] to texel coordinates
    int texX = static_cast<int>(uv[0] * texWidth);
    int texY = static_cast<int>(uv[1] * texHeight);

    int brushRadius = 15; // radius in pixels
    unsigned char brushColor[4] = { 255, 0, 0, 255 }; // RGBA red

    // Read texture into CPU memory
    std::vector<unsigned char> texData(texWidth * texHeight * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data());

    // Apply brush (simple circle)
    for (int dy = -brushRadius; dy <= brushRadius; ++dy)
    {
        for (int dx = -brushRadius; dx <= brushRadius; ++dx)
        {
            int px = texX + dx;
            int py = texY + dy;
            if (px < 0 || py < 0 || px >= texWidth || py >= texHeight) continue;
            if (dx * dx + dy * dy > brushRadius * brushRadius) continue;

            int idx = (py * texWidth + px) * 4;
            texData[idx + 0] = brushColor[0];
            texData[idx + 1] = brushColor[1];
            texData[idx + 2] = brushColor[2];
            texData[idx + 3] = brushColor[3];
        }
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, texData.data());
    /*
    glm::vec2 pos = window.GetMousePosition(true);
    std::cout << pos.x << std::endl;
    std::cout << pos.y << std::endl;

    //clip space vector
    glm::vec4 clipCoords = glm::vec4(pos.x, pos.y, -1.f, 0.f);

    //clip to eye, inverse v by inverting projection matrix
    glm::vec4 teyeCoords = clipCoords * glm::inverse(m_camera.GetViewProjectionMatrix());
    glm::vec4 eyeCoords = glm::vec4(teyeCoords.x, teyeCoords.y, -1.f, 0.f);

    //eye to world
    glm::vec4 worldCoords = eyeCoords * glm::inverse(m_camera.GetViewMatrix());
    m_mouseRay = glm::normalize(glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z));
    */
}

void ViewerApplication::UVDraw(Model model) {
    Mesh& mesh = model.GetMesh();
    Window& window = GetMainWindow();
    int width, height;
    window.GetDimensions(width, height);
    uvLookUpProgram->Use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_uvFBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    {
        // Ensure that there is a material for each submesh
        assert(mesh.GetSubmeshCount() == model.GetMaterialCount());

        Material material = *m_uvMaterial;

        material.Use();

        for (unsigned int submeshIndex = 0; submeshIndex < mesh.GetSubmeshCount(); ++submeshIndex)
        {
            glUniform1i(m_materialIDLoc, submeshIndex);
            mesh.DrawSubmesh(submeshIndex);
        }
        shaderProgram->Use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }
}

void ViewerApplication::CreateUVBuffer()
{

    Window& window = GetMainWindow();
    int width;
    int height;
    window.GetDimensions(width, height);
    
    glGenFramebuffers(1, &m_uvFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uvFBO);

    // Create UV texture (RG32F)
    GLuint uvTex;
    glGenTextures(1, &uvTex);
    glBindTexture(GL_TEXTURE_2D, uvTex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RG32F,         // store U and V as floats
        width,
        height,
        0,
        GL_RG,
        GL_FLOAT,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvTex, 0);

    // Create Material ID texture (R32I)
    GLuint matIDTex;
    glGenTextures(1, &matIDTex);
    glBindTexture(GL_TEXTURE_2D, matIDTex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R32I,          // store integer material IDs
        width,
        height,
        0,
        GL_RED_INTEGER,
        GL_INT,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, matIDTex, 0);

    // Create and attach depth buffer
    GLuint depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    // Specify both color attachments to draw to
    GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    // Check FBO completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "UV + Material ID FBO is incomplete!" << std::endl;
    }

    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}