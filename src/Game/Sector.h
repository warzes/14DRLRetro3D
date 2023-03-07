#pragma once

#include "Engine.h"

// TODO: либа триангуляции позволяет делать дырки в поверхности. возможно это пригодится - например дырки от пуль в деревянной стене
// также эта либа дает некоторые функции анализа поверхности - например нахождение внутри


// format 
// p1 - первая точка
// p2 - вторая точка
// portal - если ноль, то стена, иначе это портал, число - это ид сектора
// textureId - ид текстуры
// TODO: на самом деле стенам не нужна p1, так как у каждой последующей стены p1 равно p2 от предыдущей стены. Можно оптимизировать если будет нужно
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
};

void CompleteSector(Sector& sector); // нужно вызывать после внесения данных. В будущем подумать как лучше
void DestroySector(Sector& sector);
void UpdateSector(Sector& sector);
void DrawSector(Sector& sector);

bool IsInside(const Sector& sector, const glm::vec3& pos);