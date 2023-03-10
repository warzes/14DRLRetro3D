#include "stdafx.h"
#include "Sector.h"
//-----------------------------------------------------------------------------
struct WorldVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};
//-----------------------------------------------------------------------------
void CompleteSector(OldSector& sector)
{
	extern ShaderProgram SectorShader;

	// TODO: вместо 1 ставить реальные размеры

	sector.wallVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.wallVao = render::CreateVertexArray(&sector.wallVB, nullptr, SectorShader);

	sector.ceillingVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.ceillingVao = render::CreateVertexArray(&sector.ceillingVB, nullptr, SectorShader);

	sector.floorVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	sector.floorVao = render::CreateVertexArray(&sector.floorVB, nullptr, SectorShader);

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
			if (sector.walls[i].p2 == uniquePoint[0]) break;// но проверяем что конечная точка не равна самой первой (замыкая полигон)
			uniquePoint.push_back(sector.walls[i].p2);
		}

		// триангуляция
		TPPLPoly poly;
		poly.Init(uniquePoint.size());
		for (size_t i = 0; i < uniquePoint.size(); i++)
		{
			poly[i] = TPPLPoint{ uniquePoint[i].x, uniquePoint[i].y };
		}
		poly.SetOrientation(TPPL_ORIENTATION_CCW);

		TPPLPartition tri;
		int ret = tri.Triangulate_OPT(&poly, &sector.trianglesList);
		if (ret == 0)
		{
			//poly.Invert();
			poly.SetOrientation(TPPL_ORIENTATION_CW);
			ret = tri.Triangulate_OPT(&poly, &sector.trianglesList);
		}
		assert(ret);
	}
}
//-----------------------------------------------------------------------------
void DestroySector(OldSector& sector)
{
	render::DestroyResource(sector.wallVao);
	render::DestroyResource(sector.wallVB);
	render::DestroyResource(sector.floorVao);
	render::DestroyResource(sector.floorVB);
	render::DestroyResource(sector.ceillingVao);
	render::DestroyResource(sector.ceillingVB);

	sector.walls.clear();
}
//-----------------------------------------------------------------------------
void UpdateSector(OldSector& sector)
{
	// TODO:
}
//-----------------------------------------------------------------------------
void DrawSectors(unsigned currentId, std::vector<OldSector>& sectors)
{
	if (currentId >= sectors.size()) return;
	auto& sector = sectors[currentId];
	if (sector.walls.size() == 0) return;
	if (sector.frameId == app::GetCurrentFrameId()) return; // уже рисовался в этом кадре
	sector.frameId = app::GetCurrentFrameId();

	// draw wall
	{
		std::vector<WorldVertex> data;
		for (int i = 0; i < sector.walls.size(); i++)
		{
			const auto& w = sector.walls[i];

			float FloorHeight = sector.FloorHeight;
			float CeilingHeight = sector.CeilingHeight;

			if (w.portal > 0)
			{
				const unsigned idSector = w.portal - 1;

				if (idSector < sectors.size() && currentId != idSector)
				{
					DrawSectors(idSector, sectors);
					if (sector.FloorHeight >= sectors[idSector].FloorHeight) continue;
					else
					{
						CeilingHeight = sectors[idSector].FloorHeight;
					}
				}
			}

			float sizeWallX = glm::max(w.p1.x, w.p2.x) - glm::min(w.p1.x, w.p2.x);
			if (sizeWallX <= 0.0f)
				sizeWallX = glm::max(w.p1.y, w.p2.y) - glm::min(w.p1.y, w.p2.y);

			const float sizeWallY = CeilingHeight - FloorHeight;

			const glm::vec3 v1 = { w.p1.x, FloorHeight,   w.p1.y };
			const glm::vec3 v2 = { w.p1.x, CeilingHeight, w.p1.y };
			const glm::vec3 v3 = { w.p2.x, FloorHeight,   w.p2.y };
			const glm::vec3 v4 = { w.p1.x, CeilingHeight, w.p1.y };
			const glm::vec3 v5 = { w.p2.x, CeilingHeight, w.p2.y };
			const glm::vec3 v6 = { w.p2.x, FloorHeight,   w.p2.y };

			const glm::vec3 normal1 = glm::triangleNormal(v1, v2, v3);
			const glm::vec3 normal2 = glm::triangleNormal(v4, v5, v6);

			const glm::vec2 t1 = { 0.0f,             0.0f };
			const glm::vec2 t2 = { 0.0f,             1.0f * sizeWallY };
			const glm::vec2 t3 = { 1.0f * sizeWallX, 0.0f };
			const glm::vec2 t4 = { 1.0f * sizeWallX, 1.0f * sizeWallY };

			data.push_back({ v1, normal1, {1.0f, 1.0f, 1.0f}, t2 });
			data.push_back({ v2, normal1, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ v3, normal1, {1.0f, 1.0f, 1.0f}, t4 });
			data.push_back({ v4, normal2, {1.0f, 1.0f, 1.0f}, t1 });
			data.push_back({ v5, normal2, {1.0f, 1.0f, 1.0f}, t3 });
			data.push_back({ v6, normal2, {1.0f, 1.0f, 1.0f}, t4 });
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
				data.push_back({ v, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, t });
			}
		}

		// рисуем пол
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(sector.floorVao);

		// все тоже самое что и у пола, только изменена высота и нормаль
		for (size_t i = 0; i < data.size(); i++)
		{
			data[i].pos.y = sector.CeilingHeight;
			data[i].normal.y = -1.0f;
		}
		// рисуем потолок
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Draw(sector.floorVao);
	}
}
//-----------------------------------------------------------------------------
bool IsInside(const OldSector& sector, const glm::vec3& pos)
{
	// высота точки выше/ниже сектора
	if (pos.y < sector.FloorHeight) return false;
	if (pos.y > sector.CeilingHeight) return false;

	// точка не входит в ААББ
	if (pos.x < sector.min.x || pos.x > sector.max.x || pos.y < sector.min.y || pos.y > sector.max.y) return false;

	const glm::vec2 p = { pos.x, pos.z };

	for (auto it = sector.trianglesList.begin(); it != sector.trianglesList.end(); ++it)
	{
		const auto& t = *it;
		const glm::vec2 v1 = { t[0].x, t[0].y };
		const glm::vec2 v2 = { t[1].x, t[1].y };
		const glm::vec2 v3 = { t[2].x, t[2].y };

		if (PointInTriangle(p, v1, v2, v3)) return true;
	}
	return false;
}
//-----------------------------------------------------------------------------