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
};

void CreateWorldRender();

void DrawWorldRender();