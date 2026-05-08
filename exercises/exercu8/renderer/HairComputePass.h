#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>

#include <memory>

class Renderer;

class HairComputePass : public RenderPass
{
public:
    HairComputePass();

    void Render() override;
    const GLuint GetStrandBuffer() const { return m_strandBuffer; }


private:
    void InitComputeShader();

    std::shared_ptr<ShaderProgram> m_computeShaderProgram;
    GLuint m_strandBuffer;
};