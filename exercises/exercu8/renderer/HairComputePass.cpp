#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "HairComputePass.h"

#include "iostream"

HairComputePass::HairComputePass(std::shared_ptr<Model> headModel)
	: m_vertexBuffer(std::as_const(headModel->GetMesh()).GetVertexBuffer(0).GetHandle())
{
	std::cout << headModel->GetMesh().GetVertexBufferCount() << std::endl;
	std::cout << headModel->GetMesh().GetSubmeshCount() << std::endl;

	InitComputeShader();	
}

void HairComputePass::InitComputeShader() {

	Shader computeShader = ShaderLoader(Shader::ComputeShader).Load("shaders/hair.comp");

	m_computeShaderProgram = std::make_shared<ShaderProgram>();
	m_computeShaderProgram->Build(computeShader);

	glGenBuffers(1, &m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER,
		sizeof(glm::vec4) * 262144,
		nullptr,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
}

void HairComputePass::Render()
{
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
	m_computeShaderProgram->Use();
	// Bind buffer to binding = 0 (matches shader)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertexBuffer);
	// Dispatch compute
	glDispatchCompute(2048, 1, 1);


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}