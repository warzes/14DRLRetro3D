#include "stdafx.h"
#include "Sector.h"

struct WorldVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};

void CompleteSector(Sector& sector)
{
	extern ShaderProgram shader;

	// TODO: вместо 1 ставить реальные размеры

	sector.wallVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.wallVao = render::CreateVertexArray(&sector.wallVB, nullptr, shader);

	sector.ceillingVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.ceillingVao = render::CreateVertexArray(&sector.ceillingVB, nullptr, shader);

	sector.floorVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.floorVao = render::CreateVertexArray(&sector.floorVB, nullptr, shader);

	sector.min = glm::min(sector.walls[0].p1, sector.walls[0].p2);
	sector.max = glm::max(sector.walls[0].p1, sector.walls[0].p2);
	for (size_t i = 1; i < sector.walls.size(); i++)
	{
		sector.min = glm::min(sector.min, glm::min(sector.walls[i].p1, sector.walls[i].p2));
		sector.max = glm::max(sector.max, glm::max(sector.walls[i].p1, sector.walls[i].p2));
	}

	// Triangulate
	{
		// нужно получить список уникальных точек. 
		std::vector<glm::vec2> uniquePoint;
		// сначала берем первую точку из начала первой стены
		uniquePoint.push_back(sector.walls[0].p1);
		// все начальные точки стен равны концам предыдущих стен, поэтому берем только конечную точку p2
		for (int i = 0; i < sector.walls.size(); i++)
		{
			if (sector.walls[i].p2 == uniquePoint[0]) break;// но провер€ем что конечна€ точка не равна самой первой (замыка€ полигон)
			uniquePoint.push_back(sector.walls[i].p2);
		}

		// триангул€ци€
		TPPLPoly poly;
		poly.Init(uniquePoint.size());
		for (size_t i = 0; i < uniquePoint.size(); i++)
		{
			poly[i] = TPPLPoint{ uniquePoint[i].x, uniquePoint[i].y };
		}
		poly.SetOrientation(TPPL_ORIENTATION_CCW);

		TPPLPartition tri;
		const auto ret = tri.Triangulate_OPT(&poly, &sector.trianglesList);
		assert(ret);
	}
}

void DestroySector(Sector& sector)
{
	render::DestroyResource(sector.wallVao);
	render::DestroyResource(sector.wallVB);
	render::DestroyResource(sector.floorVao);
	render::DestroyResource(sector.floorVB);
	render::DestroyResource(sector.ceillingVao);
	render::DestroyResource(sector.ceillingVB);

	sector.walls.clear();
}

void UpdateSector(Sector& sector)
{
	// TODO:
}

void DrawSector(Sector& sector)
{
	// draw wall
	{
		std::vector<WorldVertex> data;
		for (int i = 0; i < sector.walls.size(); i++)
		{
			const auto& w = sector.walls[i];

			if (w.portal != 0) continue;

			const float sizeWallX = glm::max(w.p1.x, w.p2.x) - glm::min(w.p1.x, w.p2.x);
			const float sizeWallY = sector.CeilingHeight - sector.FloorHeight;

			const glm::vec2 t1 = { 0.0f,             0.0f };
			const glm::vec2 t2 = { 0.0f,             1.0f * sizeWallY };
			const glm::vec2 t3 = { 1.0f * sizeWallX, 0.0f };
			const glm::vec2 t4 = { 1.0f * sizeWallX, 1.0f * sizeWallY };

			data.push_back({ {w.p1.x, sector.FloorHeight,   w.p1.y}, {1.0f, 1.0f, 1.0f}, t2 });
			data.push_back({ {w.p1.x, sector.CeilingHeight, w.p1.y}, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ {w.p2.x, sector.FloorHeight,   w.p2.y}, {1.0f, 1.0f, 1.0f}, t4 });
			data.push_back({ {w.p1.x, sector.CeilingHeight, w.p1.y}, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ {w.p2.x, sector.CeilingHeight, w.p2.y}, {1.0f, 1.0f, 1.0f}, t3 });
			data.push_back({ {w.p2.x, sector.FloorHeight,   w.p2.y}, {1.0f, 1.0f, 1.0f}, t4 });
		}

		render::UpdateVertexBuffer(sector.wallVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(sector.wallVao);
	}

	// draw floor and ceiling
	{
		std::vector<WorldVertex> data;
		for (auto it = sector.trianglesList.begin(); it != sector.trianglesList.end(); ++it)
		{
			const auto& t = *it;
			assert(t.GetNumPoints() == 3);
			for (int i = 0; i < t.GetNumPoints(); i++)
			{
				glm::vec3 v = { t[i].x, sector.FloorHeight, t[i].y };
				glm::vec2 t = { sector.max.x - v.x, sector.max.y - v.z };
				data.push_back({ v, {1.0f, 1.0f, 1.0f}, t });
			}
		}
		// рисуем пол
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(sector.floorVao);

		// все тоже самое что и у пола, только изменена высота
		for (size_t i = 0; i < data.size(); i++)
		{
			data[i].pos.y = sector.CeilingHeight;
		}
		// рисуем потолок
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(sector.floorVao);
	}
}

bool IsInside(const Sector& sector, const glm::vec3& pos)
{
	// высота точки выше/ниже сектора
	if (pos.y < sector.FloorHeight) return false;
	if (pos.y > sector.CeilingHeight) return false;

	// точка не входит в јјЅЅ
	if (pos.x < sector.min.x || pos.x > sector.max.x || pos.y < sector.min.y || pos.y > sector.max.y) return false;

	for (auto it = sector.trianglesList.begin(); it != sector.trianglesList.end(); ++it)
	{
		const auto& t = *it;
		glm::vec2 v1 = { t[0].x, t[0].y };
		glm::vec2 v2 = { t[1].x, t[1].y };
		glm::vec2 v3 = { t[2].x, t[2].y };
		glm::vec2 p = { pos.x, pos.z };

		if (PointInTriangle(p, v1, v2, v3)) return true;
	}
	return false;
}