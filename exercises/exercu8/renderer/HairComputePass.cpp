#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "HairComputePass.h"

#include "iostream"

HairComputePass::HairComputePass(std::shared_ptr<Model> headModel)
	: m_vertexBuffer(std::as_const(headModel->GetMesh()).GetVertexBuffer(0).GetHandle()),
	m_indexBuffer(std::as_const(headModel->GetMesh()).GetElementBuffer(0).GetHandle())
{
	std::cout << headModel->GetMesh().GetVertexBufferCount() << std::endl;
	std::cout << headModel->GetMesh().GetSubmeshCount() << std::endl;

	//get the textures from the model material, this could probably be simplyfied
	std::shared_ptr<TextureObject> tex;
	headModel->GetMaterial(0).GetUniformValue("HairTexture", tex);
	m_hairTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);

	headModel->GetMaterial(0).GetUniformValue("PaintTexture", tex);
	m_paintTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);


	InitComputeShader();	
}

void HairComputePass::InitComputeShader() {

	Shader computeShader = ShaderLoader(Shader::ComputeShader).Load("shaders/hair.comp");

	m_computeShaderProgram = std::make_shared<ShaderProgram>();
	m_computeShaderProgram->Build(computeShader);

	m_computeShaderProgram->Use();

	m_hairLocation = m_computeShaderProgram->GetUniformLocation("HairTexture");
	m_paintLocation = m_computeShaderProgram->GetUniformLocation("PaintTexture");
	

	
	// and set them in the computeshaderProgram	
	m_computeShaderProgram->SetTexture(m_hairLocation, 0, *m_hairTexture);
	m_computeShaderProgram->SetTexture(m_paintLocation, 1, *m_paintTexture);

	// initiate strandbuffer
	glGenBuffers(1, &m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 262144 *32, nullptr, GL_DYNAMIC_DRAW);

	
}

void HairComputePass::Render()
{
	m_computeShaderProgram->Use();

	// Bind buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indexBuffer);
	// bind textures
	m_computeShaderProgram->SetTexture(m_hairLocation, 0, *m_hairTexture);
	m_computeShaderProgram->SetTexture(m_paintLocation, 1, *m_paintTexture);

	// Dispatch compute 2048 * 64 for 131 072 around as much hair as a human has, maybe I should up this because of beard
	glDispatchCompute(4096 , 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}