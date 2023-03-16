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
	std::vector<SectorEditorWall> walls; // TODO: �������� ��� ����� list ��� �������, ���� � ��� ��� ��� �� ����� ���������� �� �������, �� ����� ������� ��������� �� ��������� ����� � ��������� ����� ����� � ��������

	glm::vec2 min;
	glm::vec2 max;

	glm::vec2 position; // ����������� ������� ������� �� ���� �� ����

	float CeilingHeight = 10.0f;
	float FloorHeight = 0.0f;
	int CeilingTextureId = 0;
	int FloorTextureId = 0;

	GeometryBuffer wall;
	GeometryBuffer floor;
	GeometryBuffer ceilling;
	TPPLPolyList trianglesList;
};

// ���� � ��� ��� ��������� ����������� ���� ���� ��������� � ���������
extern bool EditorDataChange;

// ������ ��������� ����� ������
extern bool EditorNewSector;

extern std::vector<SectorEditorVertex> TempEditorVertices;
extern std::vector<SectorEditorSector> TempEditorSectors;

extern SectorEditorVertex* SelectVertex;
extern SectorEditorVertex* SelectVertex2;