#include "stdafx.h"
#include "EditorLeftGrid.h"
#include "ShaderCode.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorLeftGrid::Create()
{
	m_tempVertexBiffer.clear();
	//for( size_t i = 0; i < viewSize; i += 10 )
	{
		m_tempVertexBiffer.push_back({ glm::vec3(0.0f, 0.0f, 0.0f) });
		m_tempVertexBiffer.push_back({ glm::vec3(0.0f, 40.0f, 0.0f) });
	}

	m_geomBuff = scene::CreateGeometryBuffer(render::ResourceUsage::Static, m_tempVertexBiffer.size(), sizeof(VertexPos3), m_tempVertexBiffer.data(), LineDrawShader);

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Destroy()
{
	scene::Destroy(m_geomBuff);
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Update()
{

}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Draw(const glm::vec2& cam)
{
	glm::mat4 proj = glm::ortho(0.0f, (float)viewSize, (float)viewSize, 0.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cam.x, cam.y, -0.5f), glm::vec3(cam.x, cam.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//m_tempVertexBiffer.clear();
	//for( size_t i = 0; i < viewSize; i += 10 )
	//{
	//	m_tempVertexBiffer.push_back({ glm::vec3(0.0f, (float)i, 0.0f) });
	//	m_tempVertexBiffer.push_back({ glm::vec3(viewSize, (float)i, 0.0f) });
	//}

	render::Bind(LineDrawShader);
	render::SetUniform(UniformLineDrawProj, proj);
	//render::SetUniform(UniformLineDrawView, view);
	render::SetUniform(UniformLineDrawView, glm::mat4(1.0f));

	render::SetUniform(UniformLineDrawWorld, glm::mat4(1.0f));
	render::SetUniform(UniformLineDrawColor, glm::vec3(1.0f, 0.2f, 0.3f));

	//render::UpdateVertexBuffer(m_geomBuff.vb, 0, m_tempVertexBiffer.size(), sizeof(VertexPos3), m_tempVertexBiffer.data());

	auto& vb1 = m_geomBuff.vb;
	auto& vb2 = m_geomBuff.vao.vbo;

	render::Draw(m_geomBuff.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------