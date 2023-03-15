#include "stdafx.h"
#include "EditorLeftCursor.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
#include "EditorConstant.h"
#include "EditorLeftViewport.h"
//-----------------------------------------------------------------------------
bool EditorLeftCursor::Create()
{
	constexpr VertexPos3 v[] =
	{
		{{-0.5f,   0.0f, 0.0f}},
		{{ 0.5f,   0.0f, 0.0f}},
		{{ 0.0f,  -0.5f, 0.0f}},
		{{ 0.0f,   0.5f, 0.0f}},
	};

	m_geom = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(v), sizeof(VertexPos3), v, LineDrawShader);

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Destroy()
{
	scene::Destroy(m_geom);
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Update(const EditorLeftViewport& viewport)
{
	glm::vec2 realCursorPos = app::GetMousePosition();
	realCursorPos.y = (viewport.GetHeight() - realCursorPos.y); // TODO: что-то с этим сделать

	if( realCursorPos.x < viewport.GetWidth() )
	{
		// TODO: перенести этот код в viewport или грид
		m_posToMap = viewport.PosToWorldSpace({ realCursorPos.x, realCursorPos.y, 0.0f }, true);
		glm::ivec2 intPos = viewport.GetPosInMap({ realCursorPos.x, realCursorPos.y, 0.0f });
		//std::cout << "pos=" << std::to_string(m_pos.x) << ":" << std::to_string(m_pos.y);
		//std::cout << "    posMap=" << std::to_string(intPos.x) << ":" << std::to_string(intPos.y);

		float offsetX = m_posToMap.x / gridStep - intPos.x;
		float offsetY = m_posToMap.y / gridStep - intPos.y;

		m_posToMap = intPos * gridStep;
		if( offsetX > 0.3f && offsetX < 0.7f ) m_posToMap.x += 0.5f*gridStep;
		else if( offsetX > 0.6f ) m_posToMap.x += 1.0f * gridStep;
		if( offsetY > 0.3f && offsetY < 0.7f ) m_posToMap.y += 0.5f*gridStep;
		else if( offsetY > 0.6f ) m_posToMap.y += 1.0f*gridStep;

		m_posToScreen.x = m_posToMap.x - viewport.GetCameraPosition().x;
		m_posToScreen.y = m_posToMap.y - viewport.GetCameraPosition().y;

		//std::cout << "    npos=" << std::to_string(offsetX) << ":" << std::to_string(offsetY) << std::endl;
	}
} 
//-----------------------------------------------------------------------------
void EditorLeftCursor::Draw(const EditorLeftViewport& viewport)
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(m_posToScreen.x, m_posToScreen.y, 0));
	world = glm::scale(world, glm::vec3(3.5f));

	render::Bind(LineDrawShader);
	render::SetUniform(UniformLineDrawProj, viewport.GetOrthoProjection());
	render::SetUniform(UniformLineDrawView, glm::mat4(1.0f));
	render::SetUniform(UniformLineDrawWorld, world);
	render::SetUniform(UniformLineDrawColor, glm::vec3(1.0f, 0.8f, 0.2f));

	render::Draw(m_geom.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------