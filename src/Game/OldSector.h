#pragma once

#include "Engine.h"

// TODO: ���� ������������ ��������� ������ ����� � �����������. �������� ��� ���������� - �������� ����� �� ���� � ���������� �����
// ����� ��� ���� ���� ��������� ������� ������� ����������� - �������� ���������� ������

struct OldWallPoint
{
	glm::vec2 point;
};

// format 
// p1 - ������ �����
// p2 - ������ �����
// portal - ���� ����, �� �����, ����� ��� ������, ����� - ��� �� �������
// textureId - �� ��������
// TODO: �� ����� ���� ������ �� ����� p1, ��� ��� � ������ ����������� ����� p1 ����� p2 �� ���������� �����. ����� �������������� ���� ����� �����
struct OldWall
{
	OldWallPoint p1;
	OldWallPoint p2;
	unsigned portal;
	int textureId;
};

struct OldSector
{
	std::vector<OldWall> walls;
	float CeilingHeight;
	float FloorHeight;
	int CeilingTextureId;
	int FloorTextureId;

	// AABB
	// TODO: ��������� �������������
	glm::vec2 min;
	glm::vec2 max;

	glm::vec2 GetSize() const { return max - min; }

	// TODO: ������ �� ��������� � ������. � ���� ����� ���� ������ ��������. ������ � ��� ����� ����� ����� �����, � � ������� ����� ������ �������, ������ �������� ���������� �� Wall::textureId. � ������� ���� ������� �� ��������, ������� ������ ������� ��� ������ ��������
	VertexBuffer wallVB;
	VertexArray wallVao;

	VertexBuffer floorVB;
	VertexArray floorVao;

	VertexBuffer ceillingVB;
	VertexArray ceillingVao;

	TPPLPolyList trianglesList;

	uint32_t frameId = 999999;
};

void CompleteSector(OldSector& sector); // ����� �������� ����� ������� �������� ������. � ������� �������� ��� �����

std::vector<OldSector> LoadSectorFromFile(const char* fileName, float scale = 1.0f);
void DestroySector(OldSector& sector);
void UpdateSector(OldSector& sector);
void DrawSectors(unsigned currentId, std::vector<OldSector>& sectors, const std::vector<Texture2D>& textures);

bool IsInside(const OldSector& sector, const glm::vec3& pos);