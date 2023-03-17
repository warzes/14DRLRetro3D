#include "stdafx.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
https://www.dhpoware.com/demos/glslVertexLighting.html
//-----------------------------------------------------------------------------
bool EditorDataChange = true;
bool EditorNewSector = false;
SectorEditorVertex CurrentCursorPoint;
glm::vec3 ProbableWallColor = { 0.4f, 0.4f, 1.0f };
std::vector<SectorEditorVertex> TempEditorVertices;
std::vector<SectorEditorSector> TempEditorSectors;
SectorEditorVertex* SelectPoint = nullptr;
SectorEditorWall* SelectWall1 = nullptr;
SectorEditorWall* SelectWall2 = nullptr;
SectorEditorSector* SelectSector = nullptr;
//-----------------------------------------------------------------------------
bool IsContains(const std::vector<SectorEditorVertex>& arrayVerts, const SectorEditorVertex& vert)
{
	for( size_t i = 0; i < arrayVerts.size(); i++ )
	{
		if( vert == TempEditorVertices[i] )
			return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
void SectorEditorSector::Build()
{
	createGeometryBuffer();
	ReBuild();
}
//-----------------------------------------------------------------------------
void SectorEditorSector::ReBuild()
{
	trianglesList.clear();
	computeAABB();
	triangulate();
}
//-----------------------------------------------------------------------------
void SectorEditorSector::computeAABB()
{
	min = glm::min(walls[0].p1.pos, walls[0].p2.pos);
	max = glm::max(walls[0].p1.pos, walls[0].p2.pos);
	for( size_t i = 1; i < walls.size(); i++ )
	{
		min = glm::min(min, glm::min(walls[i].p1.pos, walls[i].p2.pos));
		max = glm::max(max, glm::max(walls[i].p1.pos, walls[i].p2.pos));
	}
}
//-----------------------------------------------------------------------------
void SectorEditorSector::createGeometryBuffer()
{
	scene::Destroy(wall);
	scene::Destroy(ceilling);
	scene::Destroy(floor);

	wall = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);
	ceilling = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);
	floor = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);
}
//-----------------------------------------------------------------------------
void SectorEditorSector::triangulate()
{
	// триангул€ци€
	TPPLPoly poly;
	poly.Init((long)walls.size());
	for( size_t i = 0; i < walls.size(); i++ )
	{
		poly[i] = TPPLPoint{ walls[i].p1.pos.x, walls[i].p1.pos.y };
	}
	poly.SetOrientation(TPPL_ORIENTATION_CCW);

	TPPLPartition tri;
	int ret = tri.Triangulate_OPT(&poly, &trianglesList);
	if( ret == 0 )
	{
		//poly.Invert();
		poly.SetOrientation(TPPL_ORIENTATION_CW);
		ret = tri.Triangulate_OPT(&poly, &trianglesList);
	}
	assert(ret);
}
//-----------------------------------------------------------------------------