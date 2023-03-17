#include "stdafx.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorDataChange = true;
bool EditorNewSector = false;
SectorEditorVertex CurrentCursorPoint;
glm::vec3 CurrentCursorWallColor = { 0.4f, 0.4f, 1.0f };
std::vector<SectorEditorVertex> TempEditorVertices;
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









std::vector<SectorEditorSector> TempEditorSectors;
SectorEditorVertex* SelectVertex = nullptr;
SectorEditorVertex* SelectVertex2 = nullptr;
//-----------------------------------------------------------------------------