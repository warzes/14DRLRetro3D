#pragma once

/*
идея такая
мир поделен на сектора
каждый сектор содержит список стен
стены состоят из двух точек
*/

// format 
// p1 - первая точка
// p2 - вторая точка
// portal - если ноль, то стена, иначе это портал, число - это ид сектора
// textureId - ид текстуры
struct Wall
{
	glm::vec2 p1;
	glm::vec2 p2;
	int portal;
	int textureId;
};

struct Sector
{
	std::vector<Wall> numWalls;
	float CeilingHeight;
	float FloorHeight;

	// AABB
	// TODO: присвоить бесконечность
	glm::vec2 min;
	glm::vec2 max;
	
	glm::vec2 GetSize() const { return max - min; }
};

void CreateWorldRender();

void DrawWorldRender();