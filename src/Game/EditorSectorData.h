#pragma once

#include "EditorConstant.h"
#include "Engine.h"

struct SectorEditorVertex
{
	glm::vec2 pos = glm::vec2(EditorMapGridSize);

	bool operator==(const glm::vec2& v) { return pos == v; }
	bool operator==(const SectorEditorVertex& r) { return pos == r.pos; }

	bool IsValid() const
	{
		return pos.x >= 0.0f && pos.y >= 0.0f && pos.x < EditorMapGridSize && pos.y < EditorMapGridSize;
	}
};

bool IsContains(const std::vector<SectorEditorVertex>& arrayVerts, const SectorEditorVertex& vert);

struct SectorEditorWall
{
	SectorEditorVertex p1;
	SectorEditorVertex p2;
	unsigned textureId = 0;

	bool IsValid() const
	{
		return p1.IsValid() && p2.IsValid();
	}
};

struct SectorEditorSector
{
	std::vector<SectorEditorWall> walls; // TODO: возможно тут лучше list или очередь, дело в том что мне не нужно обращаться по индексу, но нужно хранить указатель на выбранную стену и добавлять новые стены в середину

	glm::vec2 min;
	glm::vec2 max;

	glm::vec2 position; // высчитывать позицию сектора из стен по ААББ

	float FloorHeight = 0.0f;
	float CeilingHeight = 10.0f;

	unsigned FloorTextureId = 0;
	unsigned CeilingTextureId = 0;

	GeometryBuffer wall;
	GeometryBuffer floor;
	GeometryBuffer ceilling;
	TPPLPolyList trianglesList;
};

// идея в том что изменения производить если были изменения в редакторе
extern bool EditorDataChange;
// сейчас создается новый сектор
extern bool EditorNewSector;

extern SectorEditorVertex CurrentCursorPoint;
extern glm::vec3 CurrentCursorWallColor;
extern std::vector<SectorEditorVertex> TempEditorVertices;













extern std::vector<SectorEditorSector> TempEditorSectors;

extern SectorEditorVertex* SelectVertex;
extern SectorEditorVertex* SelectVertex2;