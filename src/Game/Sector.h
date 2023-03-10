#pragma once

#include "Engine.h"

// TODO: ���� ������������ ��������� ������ ����� � �����������. �������� ��� ���������� - �������� ����� �� ���� � ���������� �����
// ����� ��� ���� ���� ��������� ������� ������� ����������� - �������� ���������� ������


// format 
// p1 - ������ �����
// p2 - ������ �����
// portal - ���� ����, �� �����, ����� ��� ������, ����� - ��� �� �������
// textureId - �� ��������
// TODO: �� ����� ���� ������ �� ����� p1, ��� ��� � ������ ����������� ����� p1 ����� p2 �� ���������� �����. ����� �������������� ���� ����� �����
struct OldWall
{
	glm::vec2 p1;
	glm::vec2 p2;
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

void CompleteSector(OldSector& sector); // ����� �������� ����� �������� ������. � ������� �������� ��� �����
void DestroySector(OldSector& sector);
void UpdateSector(OldSector& sector);
void DrawSectors(unsigned currentId, std::vector<OldSector>& sectors);

bool IsInside(const OldSector& sector, const glm::vec3& pos);