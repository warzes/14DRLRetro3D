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
void CompleteSector(Sector& sector)
{
	extern ShaderProgram SectorShader;

	// TODO: ������ 1 ������� �������� �������

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
		// ����� �������� ������ ���������� �����. 
		std::vector<glm::vec2> uniquePoint;
		// ������� ����� ������ ����� �� ������ ������ �����
		uniquePoint.push_back(sector.walls[0].p1);
		// ��� ��������� ����� ���� ����� ������ ���������� ����, ������� ����� ������ �������� ����� p2
		for (int i = 0; i < sector.walls.size(); i++)
		{
			if (sector.walls[i].p2 == uniquePoint[0]) break;// �� ��������� ��� �������� ����� �� ����� ����� ������ (������� �������)
			uniquePoint.push_back(sector.walls[i].p2);
		}

		// ������������
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
std::vector<Sector> LoadSectorFromFile(const char* fileName, float scale)
{
	FILE *fp = fopen(fileName, "r");
	if( !fp ) 
	{ 
		LogError("Error opening level.h");
		return {};
	}

	int numText = 19; //number of textures
	int numSect = 0; //number of sectors
	int numWall = 0; //number of walls

	std::vector<Sector> sectors;

	// number of sectors
	fscanf(fp, "%i", &numSect);
	sectors.resize(numSect);

	struct numWallHelper // TODO: ��������� ����� ��� ����������� ���� � ��������
	{ 
		int ws, we; //wall number start and end
	};
	std::vector<numWallHelper> wallHelper(numSect);

	// load all sectors
	for( int s = 0; s < numSect; s++ )
	{		
		fscanf(fp, "%i", &wallHelper[s].ws);
		fscanf(fp, "%i", &wallHelper[s].we);
		// TODO: ����� ����������. � ��������� � ������� ������� ���� ����� ����, ��� ��� �� ����� � ����� �����
		//sectors[s].walls.resize(wallHelper[s].we - wallHelper[s].ws);

		int z1, z2; //height of bottom and top
		fscanf(fp, "%i", &z1);
		fscanf(fp, "%i", &z2);
		// TODO: ��� ������ � �����, � ��� ����� � �������
		sectors[s].FloorHeight = z1 * scale;
		sectors[s].CeilingHeight = z2 * scale;

		int st, ss;             //surface texture, surface scale 
		fscanf(fp, "%i", &st);
		fscanf(fp, "%i", &ss);
		
		sectors[s].CeilingTextureId = st;
		sectors[s].FloorTextureId = st;
	}

	fscanf(fp, "%i", &numWall);   //number of walls
	// TODO: ��� ��� �� �����

	// load all walls
	for( int w = 0; w < numWall; w++ )
	{
		Wall wall;

		int x1, y1;             //bottom line point 1
		fscanf(fp, "%i", &x1);
		fscanf(fp, "%i", &y1);
		wall.p1.x = x1 * scale;
		wall.p1.y = y1 * scale;

		int x2, y2;             //bottom line point 2
		fscanf(fp, "%i", &x2);
		fscanf(fp, "%i", &y2);
		wall.p2.x = x2 * scale;
		wall.p2.y = y2 * scale;
				
		wall.portal = 0;

		int wt, u, v;            //wall texture and u/v tile
		fscanf(fp, "%i", &wt);
		fscanf(fp, "%i", &u);
		fscanf(fp, "%i", &v);
		int shade;             //shade of the wall // ����?
		fscanf(fp, "%i", &shade);

		wall.textureId = wt;

		int numS = 0;
		// ���� ������ ������
		for( int i = 0; i < wallHelper.size(); i++ )
		{
			if( w < wallHelper[i].we )
			{
				numS = i;
				break;
			}
		}

		auto& sector = sectors[numS];
		sector.walls.push_back(wall);
	}

	//fscanf(fp, "%i %i %i %i %i", &P.x, &P.y, &P.z, &P.a, &P.l); //player position, angle, look direction 
	fclose(fp);

	for( int i = 0; i < sectors.size(); i++)
		CompleteSector(sectors[i]);
	return sectors;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void UpdateSector(Sector& sector)
{
	// TODO:
}
//-----------------------------------------------------------------------------
void DrawSectors(unsigned currentId, std::vector<Sector>& sectors, const std::vector<Texture2D>& textures)
{
	if (currentId >= sectors.size()) return;
	auto& sector = sectors[currentId];
	if (sector.walls.size() == 0) return;
	if (sector.frameId == app::GetCurrentFrameId()) return; // ��� ��������� � ���� �����
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
					DrawSectors(idSector, sectors, textures);
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
		render::Bind(textures[sector.walls[0].textureId]); // TODO: ������� �������� ������ ������
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

		// ������ ���
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Bind(textures[sector.FloorTextureId]);
		render::Draw(sector.floorVao);

		// ��� ���� ����� ��� � � ����, ������ �������� ������ � �������
		for (size_t i = 0; i < data.size(); i++)
		{
			data[i].pos.y = sector.CeilingHeight;
			data[i].normal.y = -1.0f;
		}
		// ������ �������
		render::UpdateVertexBuffer(sector.floorVB, 0, data.size(), sizeof(WorldVertex), data.data());
		render::Bind(textures[sector.CeilingTextureId]);
		render::Draw(sector.floorVao);
	}
}
//-----------------------------------------------------------------------------
bool IsInside(const Sector& sector, const glm::vec3& pos)
{
	// ������ ����� ����/���� �������
	if (pos.y < sector.FloorHeight) return false;
	if (pos.y > sector.CeilingHeight) return false;

	// ����� �� ������ � ����
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