#include "stdafx.h"
#include "EditorLeftCursor.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
#include "EditorConstant.h"
#include "EditorLeftViewport.h"
#include "EditorLeftGrid.h"
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

	m_geom = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(v), sizeof(VertexPos3), v, SimpleColorShader);

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Destroy()
{
	scene::Destroy(m_geom);
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Update(const EditorLeftViewport& viewport, const EditorLeftGrid& grid)
{
	grid.GetCursorToMap(viewport, m_posToMap, m_posToScreen);
} 
//-----------------------------------------------------------------------------
void EditorLeftCursor::Draw(const EditorLeftViewport& viewport)
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(m_posToScreen.x, m_posToScreen.y, 0));
	world = glm::scale(world, glm::vec3(3.5f));

	render::Bind(SimpleColorShader);
	render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
	render::SetUniform(SimpleColorDrawView, glm::mat4(1.0f));
	render::SetUniform(SimpleColorDrawWorld, world);
	render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 0.8f, 0.2f));

	render::Draw(m_geom.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------