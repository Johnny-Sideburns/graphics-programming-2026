#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "HairComputePass.h"

#include "iostream"

HairComputePass::HairComputePass()
{
	InitComputeShader();	
}

void HairComputePass::InitComputeShader() {

	Shader computeShader = ShaderLoader(Shader::ComputeShader).Load("shaders/hair.comp");

	m_computeShaderProgram = std::make_shared<ShaderProgram>();
	m_computeShaderProgram->Build(computeShader);

	glGenBuffers(1, &m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER,
		sizeof(glm::vec4) * 131072,
		nullptr,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
}

void HairComputePass::Render()
{
	m_computeShaderProgram->Use();
	// Bind buffer to binding = 0 (matches shader)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);

	// Dispatch compute
	glDispatchCompute(64, 1, 1);


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}