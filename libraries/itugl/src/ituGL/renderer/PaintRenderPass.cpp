#include <ituGL/renderer/PaintRenderPass.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>


#include "iostream"

PaintRenderPass::PaintRenderPass(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target, int drawcallCollectionIndex)
    : m_drawcallCollectionIndex(drawcallCollectionIndex),
    m_width(width), m_height(height)
{   

    m_mousePosition = std::make_shared<glm::vec2>(0.0f);
    m_brushWorldPos = std::make_shared<glm::vec3>(0.0f);
    m_brushWorldNormal = std::make_shared<glm::vec3>(0.0f);
    m_brushRadius = std::make_shared<float>(0.1f);
    m_paint = std::make_shared<bool>(false);
    
    InitDebugShaderProgram(renderer);
    std::cout << "debug shade init" << std::endl;

    InitDepthShaderProgram(renderer);
    std::cout << "brush shade init" << std::endl;

    InitUVShaderProgram(renderer);
    std::cout << "uv shade init" << std::endl;

    InitComputeShaderProgram();
    std::cout << "compute shade init" << std::endl;

    //other inits
    InitTextures(width, height);
    m_paintTexture = target;
    InitUVFramebuffer();
    InitDepthFramebuffer();

    //InitFullscreenQuad();

    std::cout << glGetString(GL_VERSION) << std::endl;
    /*
    */
}

void PaintRenderPass::InitFramebuffer()
{
    m_framebuffer.Bind();

    m_framebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);

    // Set the paint texture as color attachment 0
    m_framebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_paintTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_framebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,
        }));

    FramebufferObject::Unbind();
    assert(m_framebuffer.IsValid());
}


void PaintRenderPass::InitDepthFramebuffer()
{
    m_depthFramebuffer.Bind();

    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);
    
    // Set the paint texture as color attachment 0
    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_normalsTexture);

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
    m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_UVTexture);


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

    // set the uv texture
    m_UVTexture = std::make_shared<Texture2DObject>();
    m_UVTexture->Bind();
    m_UVTexture->SetImage(0, width, height, TextureObject::FormatRG, TextureObject::InternalFormatRG32F);
    m_UVTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_UVTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // Bind the brush texture
    m_normalsTexture = std::make_shared<Texture2DObject>();
    m_normalsTexture->Bind();
    m_normalsTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA16F);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // Bind the paint texture
    m_paintTexture = std::make_shared<Texture2DObject>();
    m_paintTexture->Bind();
    m_paintTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    m_paintTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_paintTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    Texture2DObject::Unbind();
}

void PaintRenderPass::InitDepthShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/depth.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/depth.frag");
    m_depthShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_depthShaderProgramPtr->Build(vertexShader, fragmentShader);
    std::cout << "brush shade init" << std::endl;

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_depthShaderProgramPtr->GetUniformLocation("WorldMatrix");
    //ShaderProgram::Location worldViewMatrixLocation = m_brushShaderProgramPtr->GetUniformLocation("WorldViewMatrix");
    std::cout << "brush shade init" << std::endl;
    ShaderProgram::Location worldViewProjMatrixLocation = m_depthShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    //ShaderProgram::Location mouseUVLocation = m_brushShaderProgramPtr->GetUniformLocation("MouseUV");
    //ShaderProgram::Location brushLocation = m_brushShaderProgramPtr->GetUniformLocation("BrushRadius");
    //ShaderProgram::Location paintTextureLocation = m_brushShaderProgramPtr->GetUniformLocation("PaintTexture");
    //ShaderProgram::Location brushWorldLocation = m_shaderProgramPtr->GetUniformLocation("BrushWorldPos");
    //ShaderProgram::Location uvTextureLocation = m_brushShaderProgramPtr->GetUniformLocation("UVTexture");


    renderer.RegisterShaderProgram(m_depthShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
            //shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            //shaderProgram.SetUniform(mouseUVLocation, *GetMousePosPtr());
            //shaderProgram.SetUniform(brushLocation, *GetBrushRadius());
            //shaderProgram.SetTexture(uvTextureLocation, 0, *GetUVTexture());
            //shaderProgram.SetUniform(brushWorldLocation, *GetBrushWorldPos());
            //shaderProgram.SetTexture(paintTextureLocation, 1, *GetPaintTexture());
        },
        nullptr
        
    );
    std::cout << "brush shade init" << std::endl;

}


void PaintRenderPass::InitUVShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/uvlook.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/uvdisc.frag");
    m_UVShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_UVShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_UVShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location worldViewMatrixLocation = m_UVShaderProgramPtr->GetUniformLocation("WorldViewMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_UVShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    ShaderProgram::Location mouseWorldPositionLocation = m_UVShaderProgramPtr->GetUniformLocation("MouseWP");
    ShaderProgram::Location brushLocation = m_UVShaderProgramPtr->GetUniformLocation("BrushRadius");
    ShaderProgram::Location brushNormalLocation = m_UVShaderProgramPtr->GetUniformLocation("BrushNormal");

    renderer.RegisterShaderProgram(m_UVShaderProgramPtr,
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
    m_targetTextureLocation = m_computeShaderProgramPtr->GetUniformLocation("PaintTexture");
    m_mouseUVLocation = m_computeShaderProgramPtr->GetUniformLocation("MouseUV");
    m_brushLocation = m_computeShaderProgramPtr->GetUniformLocation("BrushRadius");

}

void PaintRenderPass::Render()
{
    if (*m_paint) {
        Paint();
        *m_paint = false;
    }
    
    DebugDraw();
    
    //Unbind the framebuffer
    FramebufferObject::Unbind();

    //DebugDisplay();
    
}

void PaintRenderPass::Paint()
{
    Renderer& renderer = GetRenderer();
    SetBrushWorldPos(renderer);
    RenderUV(renderer);
    //ApplyBrushToPaintTextureCPU();

}

void PaintRenderPass::RenderUV(Renderer& renderer)
{
    const Camera& camera = renderer.GetCurrentCamera();
    Camera tcam = Camera();
    const glm::vec3 pos = *m_brushWorldPos + *m_brushWorldNormal;
    tcam.SetViewMatrix(*m_brushWorldPos,pos, glm::vec3(0.0f, 1.0f, 0.0f));

    renderer.SetCurrentCamera(tcam);

    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);

    m_uvFramebuffer.Bind();
    m_UVShaderProgramPtr->Use();

    renderer.GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    
    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {
        renderer.UpdateTransforms(m_UVShaderProgramPtr, drawcallInfo.worldMatrixIndex);

        drawcallInfo.vao.Bind();
        drawcallInfo.drawcall.Draw();
    }
    renderer.SetCurrentCamera(camera);
}

void PaintRenderPass::SetBrushWorldPos(Renderer& renderer)
{
    const Camera& camera = renderer.GetCurrentCamera();
    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);


    //paint render pass
    m_depthFramebuffer.Bind();
    m_depthShaderProgramPtr->Use();

    renderer.GetDevice().Clear(false, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    // for all drawcalls
    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {
        renderer.UpdateTransforms(m_depthShaderProgramPtr, drawcallInfo.worldMatrixIndex);

        drawcallInfo.vao.Bind();
        drawcallInfo.drawcall.Draw();
    }
    /*
    * depth get mouse pos
    */
    float x = m_mousePosition->x;
    float y = m_mousePosition->y;
    
    m_depthFramebuffer.Bind();
    /*
    */
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    *m_brushWorldPos = glm::vec3(world);

    std::cout << m_brushWorldPos->x << "," << m_brushWorldPos->y << "," << m_brushWorldPos->z << std::endl;
}



void PaintRenderPass::ApplyBrushToPaintTextureCPU()
{

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

void PaintRenderPass::InitDebugShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/debugVert.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/debugFrag.frag");
    m_debugShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_debugShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldViewMatrixLocation = m_debugShaderProgramPtr->GetUniformLocation("WorldViewMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_debugShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    m_debugPaintTextureLocation = m_debugShaderProgramPtr->GetUniformLocation("PaintTexture");


    renderer.RegisterShaderProgram(m_debugShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
        },
        nullptr
    );
}


void PaintRenderPass::DebugDisplay()
{
    glViewport(0, 0, m_width, m_height);

    // Use your debug shader
    m_debugShaderProgramPtr->Use();
    m_debugShaderProgramPtr->SetTexture(m_debugPaintTextureLocation, 0, *m_normalsTexture);

    // Minimal fullscreen quad (raw OpenGL)
    static GLuint quadVAO = 0;
    static GLuint quadVBO = 0;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions   // texcoords
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1); // texcoord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void PaintRenderPass::RenderBrush()
{
    Renderer& renderer = GetRenderer();

    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);

    //paint render pass
    m_depthFramebuffer.Bind();
    m_depthShaderProgramPtr->Use();

    renderer.GetDevice().Clear(false, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    // for all drawcalls
    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {
        renderer.UpdateTransforms(m_depthShaderProgramPtr, drawcallInfo.worldMatrixIndex);

        drawcallInfo.vao.Bind();
        drawcallInfo.drawcall.Draw();
    }
}


void PaintRenderPass::InitFullscreenQuad()
{
    if (m_quadVAO != 0) return; // already initialized

    float quadVertices[] = {
        // positions   // texcoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PaintRenderPass::RenderBrushMask()
{
    m_depthFramebuffer.Bind(); // write to brushmask texture
    glViewport(0, 0, m_width, m_height);

    m_depthShaderProgramPtr->Use();

    // bind textures
    //m_brushShaderProgramPtr->SetTexture(uvTextureLocation, 0, *m_UVTexture);
    //m_brushShaderProgramPtr->SetTexture(brushMaskLocation, 1, *m_brushMask);

    // draw fullscreen quad
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    FramebufferObject::Unbind();
}
/*
    if (*m_paint) {

        // --- COMPUTE PASS ---
        m_computeShaderProgramPtr->Use();

        // Bind UV texture (read)
        glActiveTexture(GL_TEXTURE0);
        m_UVTexture->Bind();
        glUniform1i(m_uvTextureLocation, 0);
        auto handle = std::as_const(*m_paintTexture).GetHandle();
        assert(handle != 0);
        std::cout << "handle not 0" << std::endl;
        // Bind paint texture (write)
        glBindImageTexture(1, std::as_const(*m_paintTexture).GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        // Set uniforms
        glUniform2f(m_mouseUVLocation, m_mousePosition->x, m_mousePosition->y);
        glUniform1f(m_brushLocation, *m_brushRadius);

        // Dispatch
        int groupSizeX = 16;
        int groupSizeY = 16;

        int groupsX = (m_width + groupSizeX - 1) / groupSizeX;
        int groupsY = (m_height + groupSizeY - 1) / groupSizeY;

        glDispatchCompute(groupsX, groupsY, 1);

        // IMPORTANT: sync so texture is usable after
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    }
    */