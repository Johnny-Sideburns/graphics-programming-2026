#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "PaintRenderPass.h"

#include "iostream"

PaintRenderPass::PaintRenderPass(int width, int height, Renderer& renderer, int drawcallCollectionIndex)
    : m_drawcallCollectionIndex(drawcallCollectionIndex),
    m_width(width), m_height(height)
{   

    m_mousePosition = std::make_shared<glm::vec2>(0.0f);
    m_brushWorldPos = std::make_shared<glm::vec3>(0.0f);
    m_brushWorldNormal = std::make_shared<glm::vec3>(0.0f);
    m_brushRadius = std::make_shared<float>(0.1f);
    m_paint = std::make_shared<bool>(false);
    //m_textureIdMap = std::make_shared<TextureIdMap>();

    InitHitShaderProgram(renderer);
    std::cout << "brush shade init" << std::endl;

    InitUVShaderProgram(renderer);
    std::cout << "uv shade init" << std::endl;

    //InitComputeShaderProgram();
    //std::cout << "compute shade init" << std::endl;

    //other inits
    InitTextures(width, height);
    std::cout << "compute shade init" << std::endl;

    //m_paintTexture = target;
    //std::cout << "compute shade init" << std::endl;

    InitUVFramebuffer();
    std::cout << "compute shade init" << std::endl;

    InitDepthFramebuffer();
    std::cout << "compute shade init" << std::endl;

    m_modelId = 10;

 

    int major = 0;
    int minor = 0;


    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "OpenGL Version: " << major << "," << minor << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    if (major >= 4 && minor >= 3)
    {
        m_compute = true;
    }
    else {
        m_compute = false;
    }

}

void PaintRenderPass::InitDepthFramebuffer()
{
    m_depthFramebuffer.Bind();

    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);
    
    // Set the normals texture as color attachment 0
    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_normalsTexture);

    //m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color1, *m_idTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_depthFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,
        }));
    

    FramebufferObject::Unbind();
    assert(m_depthFramebuffer.IsValid());
}


void PaintRenderPass::InitUVFramebuffer()
{
    m_uvFramebuffer.Bind();

    m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);

    // Set the uv texture as color attachment 0
    m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_paintTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_uvFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,
        }));

    FramebufferObject::Unbind();
    assert(m_uvFramebuffer.IsValid());
}

void PaintRenderPass::InitTextures(int width, int height)
{
    // bind depth buffer
    m_depthTexture = std::make_shared<Texture2DObject>();
    m_depthTexture->Bind();
    m_depthTexture->SetImage(0, width, height, TextureObject::FormatDepth, TextureObject::InternalFormatDepth);
    m_depthTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_depthTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // set the uv texture 2d 32float for precision (might be overkill)
    m_uvTexture = std::make_shared<Texture2DObject>();
    m_uvTexture->Bind();
    m_uvTexture->SetImage(0, width, height, TextureObject::FormatRG, TextureObject::InternalFormatRG32F);
    m_uvTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_uvTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // Bind the normals texture 
    m_normalsTexture = std::make_shared<Texture2DObject>();
    m_normalsTexture->Bind();
    m_normalsTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA16F);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // Bind the paint texture ... this probably isn't nescesarry when it get's rebound later
    m_paintTexture = std::make_shared<Texture2DObject>();
    m_paintTexture->Bind();
    m_paintTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    m_paintTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_paintTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
    /*
    //m_idTexture->SetImage(0, width, height, TextureObject::FormatR, TextureObject::InternalFormatR32UI);
    // Bind the id texture 1d Uint
    m_idTexture = std::make_shared<Texture2DObject>();
    m_idTexture->Bind();
    // Use the OpenGL internal format GL_R32UI because it's not included in textureobject
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    m_idTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_idTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
    */

    Texture2DObject::Unbind();
}

//the DepthShaderProgram is for an inital pass to get information for the mouse position and normal
void PaintRenderPass::InitHitShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/hit.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/hit.frag");
    m_hitShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_hitShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_hitShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_hitShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    //ShaderProgram::Location IdLocation = m_hitShaderProgramPtr->GetUniformLocation("ModelId");
    //ShaderProgram::Location IdTextureLocation = m_hitShaderProgramPtr->GetUniformLocation("IdTexture");

    renderer.RegisterShaderProgram(m_hitShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            //shaderProgram.SetUniform(IdLocation, GetModelId());
            //shaderProgram.SetTexture(IdTextureLocation, 1, *GetIdTexture());
        },
        nullptr
        
    );
}

//the uv shader program 
void PaintRenderPass::InitUVShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/uvlook.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/uvpaint.frag");
    m_uvShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_uvShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_uvShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location worldViewMatrixLocation = m_uvShaderProgramPtr->GetUniformLocation("WorldViewMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_uvShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    ShaderProgram::Location mouseWorldPositionLocation = m_uvShaderProgramPtr->GetUniformLocation("BrushWorldPos");
    ShaderProgram::Location brushLocation = m_uvShaderProgramPtr->GetUniformLocation("BrushRadius");
    ShaderProgram::Location brushNormalLocation = m_uvShaderProgramPtr->GetUniformLocation("BrushNormal");

    renderer.RegisterShaderProgram(m_uvShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
            shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            shaderProgram.SetUniform(mouseWorldPositionLocation, *GetBrushWorldPos());
            shaderProgram.SetUniform(brushLocation, *GetBrushRadius());
            shaderProgram.SetUniform(brushNormalLocation, *GetBrushWorldNormal());
        },
        nullptr
    );

}

void PaintRenderPass::InitComputeShaderProgram()
{
    // Load and build shader
    Shader computeShader = ShaderLoader(Shader::ComputeShader).Load("shaders/paint.comp");
    m_computeShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_computeShaderProgramPtr->Build(computeShader);

    // Get transform related uniform locations
    m_uvTextureLocation = m_computeShaderProgramPtr->GetUniformLocation("UVTexture");
    m_paintTextureLocation = m_computeShaderProgramPtr->GetUniformLocation("PaintTexture");

}

void PaintRenderPass::Render()
{
    if (*m_paint) {
        Paint();
        *m_paint = false;
    }
    

    FramebufferObject::Unbind();

}

void PaintRenderPass::Paint()
{
    Renderer& renderer = GetRenderer();
    SetBrushWorldPos(renderer);
    RenderUV(renderer);

}

//The render uv pass
void PaintRenderPass::RenderUV(Renderer& renderer)
{
    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);

    m_uvFramebuffer.Bind();
    m_uvShaderProgramPtr->Use();

    //renderer.GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    
    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {   
        std::shared_ptr<TextureObject> tex;
        drawcallInfo.material.GetUniformValue("ColorTexture", tex);
        m_paintTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);
        if (!m_paintTexture) continue;
        m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_paintTexture);
        renderer.UpdateTransforms(m_uvShaderProgramPtr, drawcallInfo.worldMatrixIndex);
        drawcallInfo.vao.Bind();
        drawcallInfo.drawcall.Draw();
      
    }
}

//renderpass for getting mouse position and normal from depth and normal -texture respectively
void PaintRenderPass::SetBrushWorldPos(Renderer& renderer)
{
    const Camera& camera = renderer.GetCurrentCamera();
    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);


    m_depthFramebuffer.Bind();
    m_hitShaderProgramPtr->Use();

    renderer.GetDevice().Clear(false, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    
    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {
        renderer.UpdateTransforms(m_hitShaderProgramPtr, drawcallInfo.worldMatrixIndex);  
        drawcallInfo.vao.Bind();
        drawcallInfo.drawcall.Draw();
    }

    /*
    * depth get mouse pos
    */
    float x = m_mousePosition->x;
    float y = m_mousePosition->y;
    
    //bind the depthbuffer
    //m_depthFramebuffer.Bind();

    //read attachment attachment 0 
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    //get the normal from the texture in the depth pass
    float pixel[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);

    *m_brushWorldNormal = glm::vec3(pixel[0], pixel[1], pixel[2]);

    glReadBuffer(GL_NONE);

    float depth;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);


    // NDC
    float ndcX = (2.0f * x) / m_width - 1.0f;
    float ndcY = (2.0f * y) / m_height - 1.0f;
    float ndcZ = depth * 2.0f - 1.0f;

    // Clip space
    glm::vec4 clip(ndcX, ndcY, ndcZ, 1.0f);

    glm::vec4 world = glm::inverse(camera.GetViewProjectionMatrix()) * clip;
    world /= world.w;
    
    *m_brushWorldPos = glm::vec3(world.x, world.y, world.z);


    std::cout << m_brushWorldPos->x << "," << m_brushWorldPos->y << "," << m_brushWorldPos->z << std::endl;

}


//cpu painting from uv's to texture
void PaintRenderPass::ApplyBrushToPaintTextureCPU()
{
    if (m_paintTexture == nullptr) return;
    // Bind UV mask framebuffer for read
    glBindFramebuffer(GL_FRAMEBUFFER, std::as_const(m_uvFramebuffer).GetHandle());

    std::vector<glm::vec2> brushUVs(m_width * m_height);
    glReadPixels(0, 0, m_width, m_height, GL_RG, GL_FLOAT, brushUVs.data());

    // Collect only pixels with UVs inside the circle
    std::vector<glm::vec2> pixelsToPaint;
    for (int i = 0; i < m_width * m_height; ++i) {
        glm::vec2 uv = brushUVs[i];
        if (uv != glm::vec2(0.0f)) { // or some small epsilon
            pixelsToPaint.push_back(uv);
        }
    }

    //Map UVs to paint texture pixels
    std::vector<glm::ivec2> texPixels;
    int texWidth = m_width; // m_paintTexture->GetWidth();
    int texHeight = m_height; //m_paintTexture->GetHeight();

    for (auto uv : pixelsToPaint) {
        int x = static_cast<int>(uv.x * texWidth);
        int y = static_cast<int>(uv.y * texHeight);
        texPixels.push_back(glm::ivec2(x, y));
    }

    //CPU write using glTexSubImage2D
    glBindTexture(GL_TEXTURE_2D, std::as_const(*m_paintTexture).GetHandle());

    for (auto p : texPixels) {
        unsigned char color[4] = { 255, 0, 0, 255 }; // brush color
        glTexSubImage2D(GL_TEXTURE_2D, 0, p.x, p.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    }
}

void PaintRenderPass::DebugDraw()
{
    m_uvFramebuffer.Bind(FramebufferObject::Target::Read);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);

    glBlitFramebuffer(
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
}
