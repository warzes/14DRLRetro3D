#include "stdafx.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorDataChange = true;
bool EditorNewSector = false;
std::vector<SectorEditorVertex> TempEditorVertices;
std::vector<SectorEditorSector> TempEditorSectors;
SectorEditorVertex* SelectVertex = nullptr;
SectorEditorVertex* SelectVertex2 = nullptr;
//-----------------------------------------------------------------------------