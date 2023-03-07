#include "stdafx.h"
#include "WorldRender.h"

// TODO: возможно координаты мира не могут быть меньше нуля чтобы не создавать проблемы
// unit - единица из которй вычисляется все остальное - например текстурные координаты

Sector tempSector;

void CreateWorldRender()
{
	tempSector.FloorHeight = 0.0f;
	tempSector.CeilingHeight = 3.0f;

	tempSector.walls.push_back({ .p1 = {0.0f, 0.0f}, .p2 = {4.0f, 4.0f}, .portal = 0, .textureId = 1 });
	tempSector.walls.push_back({ .p1 = {4.0f, 4.0f}, .p2 = {8.0f, 4.0f}, .portal = 0, .textureId = 2 });
	tempSector.walls.push_back({ .p1 = {8.0f, 4.0f}, .p2 = {12.0f, 0.0f}, .portal = 0, .textureId = 3 });
	tempSector.walls.push_back({ .p1 = {12.0f, 0.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 3 });
	tempSector.walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {4.0f, -4.0f}, .portal = 1, .textureId = 3 });
	tempSector.walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {0.0f, 0.0f}, .portal = 0, .textureId = 3 });

	CompleteSector(tempSector);
}

float t = 0.0f;

void DrawWorldRender()
{
	tempSector.FloorHeight = t;

	if (app::IsKeyPressed(app::KEY_Q))
		t += 0.5f;
	if (app::IsKeyPressed(app::KEY_E))
		t -= 0.5f;

	DrawSector(tempSector);	

	extern scene::Camera cam;
	if (IsInside(tempSector, cam.position))
	{
		LogPrint("Is inside");
	}
	else
		LogPrint("Is outside");
}