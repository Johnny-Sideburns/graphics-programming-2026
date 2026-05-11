#include "HairRenderPass.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/Camera.h>

#include <vector>

HairRenderPass::HairRenderPass(Renderer& renderer, GLuint strandBuffer)
	: m_strandBuffer(strandBuffer)
{
	//InitShader();
	m_strandCount = 131072;
	m_segments = 12;
	InitShader(renderer);
	glGenVertexArrays(1, &m_dummyVAO);
}

void HairRenderPass::InitShader(Renderer& renderer)
{
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version430.glsl");
    vertexShaderPaths.push_back("shaders/hair_render.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);


    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version430.glsl");
    //fragmentShaderPaths.push_back("shaders/utils.glsl");
    //fragmentShaderPaths.push_back("shaders/lambert-ggx.glsl");
    //fragmentShaderPaths.push_back("shaders/lighting.glsl");
    fragmentShaderPaths.push_back("shaders/hair_fragment.frag");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    m_shaderProgramPtr = std::make_shared<ShaderProgram>();
    m_shaderProgramPtr->Build(vertexShader, fragmentShader);

    //ShaderProgram::Location viewProjMatrixLocation = m_shaderProgramPtr->GetUniformLocation("ViewProjMatrix");
    //ShaderProgram::Location segmentsLocation = m_shaderProgramPtr->GetUniformLocation("segments");

    m_viewProjLocation = m_shaderProgramPtr->GetUniformLocation("ViewProjMatrix");
    m_segmentsLocation = m_shaderProgramPtr->GetUniformLocation("Segments");
    m_cameraPositionLocation = m_shaderProgramPtr->GetUniformLocation("CameraPosition");
    
}

void HairRenderPass::Render()
{
    Renderer& renderer = GetRenderer();
    Camera camera = renderer.GetCurrentCamera();


    glBindVertexArray(m_dummyVAO);

    m_shaderProgramPtr->Use();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);

    // Set uniforms
    m_shaderProgramPtr->SetUniform(m_viewProjLocation, camera.GetViewProjectionMatrix());
    m_shaderProgramPtr->SetUniform(m_segmentsLocation, m_segments);
    m_shaderProgramPtr->SetUniform(m_cameraPositionLocation, camera.ExtractTranslation());
    
    // Draw
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_segments * 2, m_strandCount);
    glBindVertexArray(0);
    
}