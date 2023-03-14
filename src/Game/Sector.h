#pragma once

#include "Engine.h"

// TODO: либа триангуляции позволяет делать дырки в поверхности. возможно это пригодится - например дырки от пуль в деревянной стене
// также эта либа дает некоторые функции анализа поверхности - например нахождение внутри

struct WallPoint
{
	glm::vec2 point;
};

// format 
// p1 - первая точка
// p2 - вторая точка
// portal - если ноль, то стена, иначе это портал, число - это ид сектора
// textureId - ид текстуры
// TODO: на самом деле стенам не нужна p1, так как у каждой последующей стены p1 равно p2 от предыдущей стены. Можно оптимизировать если будет нужно
struct Wall
{
	WallPoint p1;
	WallPoint p2;
	unsigned portal;
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

void CompleteSector(Sector& sector); // нужно вызывать после ручного внесения данных. В будущем подумать как лучше

std::vector<Sector> LoadSectorFromFile(const char* fileName, float scale = 1.0f);
void DestroySector(Sector& sector);
void UpdateSector(Sector& sector);
void DrawSectors(unsigned currentId, std::vector<Sector>& sectors, const std::vector<Texture2D>& textures);

bool IsInside(const Sector& sector, const glm::vec3& pos);