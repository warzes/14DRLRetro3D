#include "stdafx.h"
#include "WorldRender.h"
#include "Engine.h"

struct WorldVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};
VertexBuffer worldVB;
VertexArray worldVao;

VertexBuffer worldVB2;
VertexArray worldVao2;

Sector tempSector;

void CreateWorldRender()
{
	tempSector.FloorHeight = 0.0f;
	tempSector.CeilingHeight = 3.0f;

	tempSector.numWalls.push_back({ .p1 = {0.0f, 0.0f}, .p2 = {4.0f, 4.0f}, .portal = 0, .textureId = 1 });
	tempSector.numWalls.push_back({ .p1 = {4.0f, 4.0f}, .p2 = {8.0f, 4.0f}, .portal = 0, .textureId = 2 });
	tempSector.numWalls.push_back({ .p1 = {8.0f, 4.0f}, .p2 = {12.0f, 0.0f}, .portal = 0, .textureId = 3 });
	tempSector.numWalls.push_back({ .p1 = {12.0f, 0.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 3 });
	tempSector.numWalls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {4.0f, -4.0f}, .portal = 0, .textureId = 3 });
	tempSector.numWalls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {0.0f, 0.0f}, .portal = 0, .textureId = 3 });





	extern ShaderProgram shader;

	worldVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	worldVao = render::CreateVertexArray(&worldVB, nullptr, shader);

	worldVB2 = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	worldVao2 = render::CreateVertexArray(&worldVB2, nullptr, shader);

}

float t = 0.0f;

void DrawWorldRender()
{
	tempSector.FloorHeight = t;

	if (app::IsKeyPressed(app::KEY_Q))
		t += 0.5f;
	if (app::IsKeyPressed(app::KEY_E))
		t -= 0.5f;


	// draw wall
	{
		std::vector<WorldVertex> data;

		for (int i = 0; i < tempSector.numWalls.size(); i++)
		{
			const auto& w = tempSector.numWalls[i];

			if (w.portal != 0) continue;

			glm::vec2 minW = glm::min(w.p1, w.p2);
			glm::vec2 maxW = glm::max(w.p1, w.p2);

			float sizeWallX = maxW.x - minW.x;
			float sizeWallY = tempSector.CeilingHeight - tempSector.FloorHeight;

			glm::vec2 t1 = { 0.0f,             0.0f };
			glm::vec2 t2 = { 0.0f,             1.0f * sizeWallY };
			glm::vec2 t3 = { 1.0f * sizeWallX, 0.0f };
			glm::vec2 t4 = { 1.0f * sizeWallX, 1.0f * sizeWallY };
					

			data.push_back({ {w.p1.x, tempSector.FloorHeight, w.p1.y}, {1.0f, 1.0f, 1.0f}, t2 });
			data.push_back({ {w.p1.x, tempSector.CeilingHeight, w.p1.y}, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ {w.p2.x, tempSector.FloorHeight, w.p2.y}, {1.0f, 1.0f, 1.0f}, t4 });
			data.push_back({ {w.p1.x, tempSector.CeilingHeight, w.p1.y}, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ {w.p2.x, tempSector.CeilingHeight, w.p2.y}, {1.0f, 1.0f, 1.0f}, t3 });
			data.push_back({ {w.p2.x, tempSector.FloorHeight, w.p2.y}, {1.0f, 1.0f, 1.0f}, t4 });
		}

		render::UpdateVertexBuffer(worldVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(worldVao);
	}

	// draw floor
	{
		// нужно получить список уникальных точек. 
		std::vector<glm::vec2> uniquePoint;
		// сначала берем первую точку из начала первой стены
		uniquePoint.push_back(tempSector.numWalls[0].p1);
		// все начальные точки стен равны концам предыдущих стен, поэтому берем только конечную точку p2
		for (int i = 0; i < tempSector.numWalls.size(); i++)
		{
			if (tempSector.numWalls[i].p2 == uniquePoint[0]) break;// но проверяем что конечная точка не равна самой первой (замыкая полигон)
			uniquePoint.push_back(tempSector.numWalls[i].p2);
		}

		// 2D AABB
		glm::vec2 A1(INFINITY);
		glm::vec2 A2(0.0f);
		glm::vec2 size;
		glm::vec2 halfSize;

		TPPLPoly poly;
		poly.Init(uniquePoint.size());
		for (size_t i = 0; i < uniquePoint.size(); i++)
		{
			poly[i] = TPPLPoint{ uniquePoint[i].x, uniquePoint[i].y };


			if (A1.x > uniquePoint[i].x) A1.x = uniquePoint[i].x;
			if (A1.y > uniquePoint[i].y) A1.y = uniquePoint[i].y;

			if (A2.x < uniquePoint[i].x) A2.x = uniquePoint[i].x;
			if (A2.y < uniquePoint[i].y) A2.y = uniquePoint[i].y;
		}

		size.x = A2.x - A1.x;
		size.y = A2.y - A1.y;
		halfSize = size / 2.0f;

		poly.SetOrientation(TPPL_ORIENTATION_CCW);
		TPPLPolyList polyFinal;

		TPPLPartition tri;
		auto ret = tri.Triangulate_OPT(&poly, &polyFinal);

		std::vector<WorldVertex> data;

		for (auto it = polyFinal.begin(); it != polyFinal.end(); ++it)
		{
			auto t = *it;

			glm::vec3 v1 = { t[0].x, tempSector.FloorHeight, t[0].y };
			glm::vec3 v2 = { t[1].x, tempSector.FloorHeight, t[1].y };
			glm::vec3 v3 = { t[2].x, tempSector.FloorHeight, t[2].y };

			glm::vec2 t1 = { size.x - v1.x + A1.x, size.y - v1.z + A1.y };
			glm::vec2 t2 = { size.x - v2.x + A1.x, size.y - v2.z + A1.y };
			glm::vec2 t3 = { size.x - v3.x + A1.x, size.y - v3.z + A1.y };

			//t1 = t1/size;
			//t2 = t2/size;
			//t3 = t3/size;

			data.push_back({v1, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({v2, {1.0f, 1.0f, 1.0f}, t2 });
			data.push_back({v3, {1.0f, 1.0f, 1.0f}, t3 });
		}
		
		render::UpdateVertexBuffer(worldVB2, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(worldVao2);
	}
	
}