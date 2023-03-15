#include "stdafx.h"
#include "EditorLeftMap.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
#include "EditorLeftViewport.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorLeftMap::Create()
{
	constexpr VertexPos3 vert[] =
	{
		{{-0.5f, -0.5f, .0f}},
		{{-0.5f,  0.5f, .0f}},
		{{ 0.5f,  0.5f, .0f}},
		{{ 0.5f, -0.5f, .0f}}
	};

	constexpr int indexs[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	m_geomPoint = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(VertexPos3), vert, Countof(indexs), sizeof(indexs[0]), indexs, SimpleColorShader);
	if (!scene::IsValid(m_geomPoint))
		return false;

	constexpr VertexPos3 vert2[] = { {glm::vec3(1.0f)}, {glm::vec3(1.0f)} };
	m_geomWall = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vert2), sizeof(VertexPos3), vert2, Simple2DLineDrawShader);
	if (!scene::IsValid(m_geomWall))
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Destroy()
{
	scene::Destroy(m_geomPoint);
	scene::Destroy(m_geomWall);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Draw(const EditorLeftViewport& viewport) const
{
	render::Bind(SimpleColorShader);
	render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
	render::SetUniform(SimpleColorDrawView, viewport.GetView());
	render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 0.9f, 0.1f));

	// первая точка
	if (TempEditorCurrentWall.p1.pos.x < gridSize && TempEditorCurrentWall.p1.pos.y < gridSize
		&& TempEditorCurrentWall.p2.pos.x >= gridSize && TempEditorCurrentWall.p2.pos.y >= gridSize)
		drawPoint(TempEditorCurrentWall.p1.pos);

	for (const auto& it : TempEditorWalls)
	{
		// p1
		if (it.p1.pos.x < gridSize && it.p1.pos.y < gridSize)
			drawPoint(it.p1.pos);

		// p2
		if (it.p2.pos.x < gridSize && it.p2.pos.y < gridSize)
			drawPoint(it.p2.pos);
	}
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawPoint(const glm::vec2& pos) const
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
	world = glm::scale(world, glm::vec3(1.2f));
	render::SetUniform(SimpleColorDrawWorld, world);
	render::Draw(m_geomPoint.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------