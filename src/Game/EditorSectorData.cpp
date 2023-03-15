#include "stdafx.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorDataChange = true;
SectorEditorWall TempEditorCurrentWall = {glm::vec2(gridSize)};
std::list<SectorEditorWall> TempEditorWalls;
unsigned TempEditorVertexNum = 0;
//-----------------------------------------------------------------------------