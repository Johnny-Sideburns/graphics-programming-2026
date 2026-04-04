#pragma once

#include <ituGL/renderer/Renderer.h>
#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/shader/Material.h>
#include "renderer/PaintRenderPass.h"

#include <ituGL/application/Window.h>


class Painter
{
public:
	Painter();
	Painter(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target );
	void Update(const Window& window, float deltaTime);
	void Paint(const Window& window);
	//void InitializeShaderProgram(Renderer& renderer);
	//void InitializeMaterial(Renderer& renderer);

protected:

private:
	std::unique_ptr<PaintRenderPass> m_paintRenderPass;
	std::shared_ptr<Texture2DObject> m_depthTexture;
	std::shared_ptr<Texture2DObject> m_paintTexture;
	std::shared_ptr<bool> m_paint;
	float m_delay = 0;

	std::shared_ptr<Texture2DObject> m_targetTexture;

	std::shared_ptr<ShaderProgram> m_shaderProgramPtr;
	std::shared_ptr<glm::vec2> m_mousePosition;

};