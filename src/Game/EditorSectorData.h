#pragma once

#include "EditorConstant.h"

struct SectorEditorVertex
{
	glm::vec2 pos = glm::vec2(gridSize);

	bool IsValid() const
	{
		return pos.x < gridSize && pos.y < gridSize;
	}
};

struct SectorEditorWall
{
	SectorEditorVertex p1;
	SectorEditorVertex p2;
};


// идея в том что изменения производить если были изменения в редакторе
extern bool EditorDataChange;

extern std::vector<SectorEditorVertex> TempEditorVertices;