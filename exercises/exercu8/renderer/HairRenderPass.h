#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>
#include <memory>
class Renderer;

class HairRenderPass : public RenderPass
{
public:
	HairRenderPass(Renderer& renderer, GLuint strandBuffer);
	void InitShader(Renderer& renderer);

	void Render() override;

private:
	GLuint m_strandBuffer;
	GLuint m_dummyVAO;

	int m_strandCount = 0;
	int m_segments = 0;

	std::shared_ptr<ShaderProgram> m_shaderProgramPtr;

	ShaderProgram::Location m_viewProjLocation;
	ShaderProgram::Location m_segmentsLocation;
	ShaderProgram::Location m_cameraPositionLocation;
};