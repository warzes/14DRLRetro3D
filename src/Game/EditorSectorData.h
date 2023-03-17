#pragma once

#include "EditorConstant.h"
#include "Engine.h"








struct SectorEditorVertex
{
	glm::vec2 pos = glm::vec2(EditorMapGridSize);

	bool IsValid() const
	{
		return pos.x < EditorMapGridSize && pos.y < EditorMapGridSize;
	}
};

struct SectorEditorWall
{
	SectorEditorVertex p1;
	SectorEditorVertex p2;
	unsigned portal = 0;
	int textureId = 0;
};

struct SectorEditorSector
{
	std::vector<SectorEditorWall> walls; // TODO: возможно тут лучше list или очередь, дело в том что мне не нужно обращаться по индексу, но нужно хранить указатель на выбранную стену и добавлять новые стены в середину

	glm::vec2 min;
	glm::vec2 max;

	glm::vec2 position; // высчитывать позицию сектора из стен по ААББ

	float CeilingHeight = 10.0f;
	float FloorHeight = 0.0f;
	int CeilingTextureId = 0;
	int FloorTextureId = 0;

	GeometryBuffer wall;
	GeometryBuffer floor;
	GeometryBuffer ceilling;
	TPPLPolyList trianglesList;
};

// идея в том что изменения производить если были изменения в редакторе
extern bool EditorDataChange;

// сейчас создается новый сектор
extern bool EditorNewSector;

extern std::vector<SectorEditorVertex> TempEditorVertices;
extern std::vector<SectorEditorSector> TempEditorSectors;

extern SectorEditorVertex* SelectVertex;
extern SectorEditorVertex* SelectVertex2;