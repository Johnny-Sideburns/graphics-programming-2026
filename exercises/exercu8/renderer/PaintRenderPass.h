#pragma once

#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>
#include "../TextureIdMap.h"

#include <memory>

class Renderer;

class PaintRenderPass : public RenderPass
{
public:
    PaintRenderPass(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target, int drawcallCollectionIndex = 0);

    void Render() override;

    const std::shared_ptr<Texture2DObject> GetDepthTexture() const { return m_depthTexture; }
    const std::shared_ptr<Texture2DObject> GetPaintTexture() const { return m_paintTexture; }
    const std::shared_ptr<Texture2DObject> GetUVTexture() const { return m_uvTexture; }
    const std::shared_ptr<Texture2DObject> GetNormalsTexture() const { return m_normalsTexture; }
    const std::shared_ptr<Texture2DObject> GetIdTexture() const { return m_idTexture; }

    const std::shared_ptr<glm::vec3> GetBrushWorldPos() const { return m_brushWorldPos; }
    const std::shared_ptr<glm::vec3> GetBrushWorldNormal() const { return m_brushWorldNormal; }
    const std::shared_ptr<glm::vec2> GetMousePosPtr() const { return m_mousePosition; }
    const std::shared_ptr<float> GetBrushRadius() const { return m_brushRadius; }
    const std::shared_ptr<bool> GetPaintPtr() const { return m_paint; }
    const unsigned int GetModelId() const { return m_modelId; }

private:
    void Paint();
    void SetBrushWorldPos(Renderer& renderer);

    //void InitShaderProgram(Renderer& renderer);
    void InitUVShaderProgram(Renderer& renderer);
    void InitHitShaderProgram(Renderer& renderer);

    void RenderUV(Renderer& renderer);
    void RenderBrush();

    void ApplyBrushToPaintTextureCPU();

    void InitComputeShaderProgram();

    void InitTextures(int width, int height);
    void InitFramebuffer();
    void InitUVFramebuffer();
    void InitDepthFramebuffer();
    void DebugDraw();
    
    void InitDebugShaderProgram(Renderer& renderer);
    void DebugDisplay();

    void InitFullscreenQuad();
    void RenderBrushMask();
    void PaintComputeGPU();

private:
    int m_drawcallCollectionIndex;

    std::shared_ptr<Texture2DObject> m_depthTexture;
    std::shared_ptr<Texture2DObject> m_paintTexture;
    std::shared_ptr<Texture2DObject> m_uvTexture;
    std::shared_ptr<Texture2DObject> m_normalsTexture;
    std::shared_ptr<Texture2DObject> m_idTexture;
    
    std::shared_ptr<TextureIdMap> m_textureIdMap;
    std::shared_ptr<ShaderProgram> m_uvShaderProgramPtr;
    std::shared_ptr<ShaderProgram> m_hitShaderProgramPtr;
    //todo implement compute shader
    std::shared_ptr<ShaderProgram> m_computeShaderProgramPtr;


    std::shared_ptr<glm::vec3> m_brushWorldPos;
    std::shared_ptr<glm::vec3> m_brushWorldNormal;

    std::shared_ptr<glm::vec2> m_mousePosition;
    std::shared_ptr<float> m_brushRadius;
    std::shared_ptr<bool> m_paint;
    bool m_compute;
    unsigned int m_modelId;
    unsigned int m_texId;

    ShaderProgram::Location m_paintTextureLocation;
    ShaderProgram::Location m_uvTextureLocation;

    FramebufferObject m_framebuffer;
    FramebufferObject m_uvFramebuffer;
    FramebufferObject m_depthFramebuffer;

    GLuint m_quadVAO = 0;
    GLuint m_quadVBO = 0;


    int m_width;
    int m_height;

    std::shared_ptr<ShaderProgram> m_debugShaderProgramPtr;
    ShaderProgram::Location m_debugPaintTextureLocation;
};
