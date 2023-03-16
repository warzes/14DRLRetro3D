#pragma once

#include "Engine.h"

// TODO: либа триангуляции позволяет делать дырки в поверхности. возможно это пригодится - например дырки от пуль в деревянной стене
// также эта либа дает некоторые функции анализа поверхности - например нахождение внутри

struct OldWallPoint
{
	glm::vec2 point;
};

// format 
// p1 - первая точка
// p2 - вторая точка
// portal - если ноль, то стена, иначе это портал, число - это ид сектора
// textureId - ид текстуры
// TODO: на самом деле стенам не нужна p1, так как у каждой последующей стены p1 равно p2 от предыдущей стены. Можно оптимизировать если будет нужно
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
	// TODO: присвоить бесконечность
	glm::vec2 min;
	glm::vec2 max;

	glm::vec2 GetSize() const { return max - min; }

	// TODO: вопрос по текстурам и стенам. У стен могут быть разные текстуры. Сейчас я все стены рисую одним дипом, а в шейдере будет массив текстур, нужная текстура выбирается по Wall::textureId. В будущем если вариант не подойдет, сделать массив буферов под каждую текстуру
	VertexBuffer wallVB;
	VertexArray wallVao;

	VertexBuffer floorVB;
	VertexArray floorVao;

	VertexBuffer ceillingVB;
	VertexArray ceillingVao;

	TPPLPolyList trianglesList;

	uint32_t frameId = 999999;
};

void CompleteSector(OldSector& sector); // нужно вызывать после ручного внесения данных. В будущем подумать как лучше

std::vector<OldSector> LoadSectorFromFile(const char* fileName, float scale = 1.0f);
void DestroySector(OldSector& sector);
void UpdateSector(OldSector& sector);
void DrawSectors(unsigned currentId, std::vector<OldSector>& sectors, const std::vector<Texture2D>& textures);

bool IsInside(const OldSector& sector, const glm::vec3& pos);