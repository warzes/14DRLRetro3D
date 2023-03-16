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

	constexpr float vertData[] = { 0.0f, 1.0f };
	m_geomWall = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vertData), sizeof(float), vertData, Simple2DLineDrawShader);
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
	// draw point
	render::Bind(SimpleColorShader);
	render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
	render::SetUniform(SimpleColorDrawView, viewport.GetView());
	render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 0.9f, 0.1f));
	for( auto& it : TempEditorVertices )
	{
		if ( it.IsValid()) drawPoint(it.pos);
	}

	// draw line
	render::Bind(Simple2DLineDrawShader);
	render::SetUniform(Simple2DLineDrawViewProj, viewport.GetOrthoProjection() * viewport.GetView());
	render::SetUniform(Simple2DLineDrawColor, glm::vec3(1.0f, 0.9f, 0.1f));
	for( size_t i = 0; i < TempEditorVertices.size(); i++ )
	{
		if( i + 1 < TempEditorVertices.size() )
		{
			drawLine(TempEditorVertices[i].pos, TempEditorVertices[i+1].pos);
		}
	}

	// draw sectors
	drawSectors(viewport);
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
void EditorLeftMap::drawLine(const glm::vec2& pos1, const glm::vec2& pos2) const
{
	render::SetUniform(Simple2DLineDrawPos, glm::vec4(pos1, pos2));
	render::Draw(m_geomWall.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawSectors(const EditorLeftViewport& viewport) const
{
	for( size_t i = 0; i < TempEditorSectors.size(); i++ )
	{
		for( size_t j = 0; j < TempEditorSectors[i].walls.size(); j++ )
		{
			// draw wall
			render::Bind(Simple2DLineDrawShader);
			render::SetUniform(Simple2DLineDrawViewProj, viewport.GetOrthoProjection() * viewport.GetView());
			render::SetUniform(Simple2DLineDrawColor, glm::vec3(0.8f, 0.9f, 0.7f));
			drawLine(TempEditorSectors[i].walls[j].p1.pos, TempEditorSectors[i].walls[j].p2.pos);

			render::Bind(SimpleColorShader);
			render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
			render::SetUniform(SimpleColorDrawView, viewport.GetView());
			render::SetUniform(SimpleColorDrawColor, glm::vec3(0.8f, 0.9f, 0.7f));
			drawPoint(TempEditorSectors[i].walls[j].p1.pos);
		}
	}
}
//-----------------------------------------------------------------------------