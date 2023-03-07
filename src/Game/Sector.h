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
struct Wall
{
	glm::vec2 p1;
	glm::vec2 p2;
	int portal;
	int textureId;
};

struct Sector
{
	std::vector<Wall> walls;
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
};

void CompleteSector(Sector& sector); // ����� �������� ����� �������� ������. � ������� �������� ��� �����
void DestroySector(Sector& sector);
void UpdateSector(Sector& sector);
void DrawSector(Sector& sector);

bool IsInside(const Sector& sector, const glm::vec3& pos);