#include "stdafx.h"
#include "EditorLeftCommand.h"
#include "EditorLeftCursor.h"
#include "EditorSectorData.h"
#include "EditorLeftMap.h"
#include "Engine.h"
#include "EditorConstant.h"
#include "ShaderCode.h"
#include "VertexFormat.h"
//-----------------------------------------------------------------------------
void EditorLeftCommand::Update(const EditorLeftCursor& cursor, EditorLeftMap& map)
{
	// щелчок левой кнопкой
	glm::vec2 posInMap = cursor.PosToMap();

	if( app::IsKeyPressed(app::KEY_BACKSPACE) )
	{
		// удалить последнюю вершину
		if ( TempEditorVertices.size() > 0 )
			TempEditorVertices.pop_back();
	}

	if (app::IsMouseButtonReleased(0))
	{
		if ( posInMap.x >= 0.0f && posInMap.x < EditorMapGridSize && posInMap.y >= 0.0f && posInMap.y < EditorMapGridSize )
		{
			// сектор замыкается?
			if( TempEditorVertices.size() > 2 && posInMap == TempEditorVertices[0].pos )
			{
				buildEditorSector();
			}
			else
			{
				// точка на другой точке этого сектора? это ошибка
				bool isError = false;
				for( size_t i = 0; i < TempEditorVertices.size(); i++ )
				{
					if( posInMap == TempEditorVertices[i].pos )
					{
						isError = true;
						break;
					}
				}

				// добавляем точку если нет ошибок
				if( !isError )
				{
					TempEditorVertices.push_back({ posInMap });
					EditorDataChange = true;
					EditorNewSector = true;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void EditorLeftCommand::buildEditorSector()
{
	assert(TempEditorVertices.size() >= 3);
	EditorDataChange = true;
	EditorNewSector = false;

	SectorEditorSector sector;
	for( size_t i = 1; i < TempEditorVertices.size(); i++ )
	{
		assert(TempEditorVertices[i].IsValid());

		SectorEditorWall wall;
		wall.p1 = TempEditorVertices[i-1];
		wall.p2 = TempEditorVertices[i];

		sector.walls.push_back(wall);
	}
	// вставляем стену которая соединена с первой точкой
	SectorEditorWall wall;
	wall.p1 = TempEditorVertices[TempEditorVertices.size() - 1];
	wall.p2 = TempEditorVertices[0];
	sector.walls.push_back(wall);

	buildGeomSector(sector);

	TempEditorSectors.push_back(sector);
	TempEditorVertices.clear();
}
//-----------------------------------------------------------------------------
void EditorLeftCommand::buildGeomSector(SectorEditorSector& sector)
{
	sector.min = glm::min(sector.walls[0].p1.pos, sector.walls[0].p2.pos);
	sector.max = glm::max(sector.walls[0].p1.pos, sector.walls[0].p2.pos);
	for( size_t i = 1; i < sector.walls.size(); i++ )
	{
		sector.min = glm::min(sector.min, glm::min(sector.walls[i].p1.pos, sector.walls[i].p2.pos));
		sector.max = glm::max(sector.max, glm::max(sector.walls[i].p1.pos, sector.walls[i].p2.pos));
	}

	sector.wall = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);
	sector.ceilling = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);
	sector.floor = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr, SectorRenderShader);

	// Triangulate
	{
		// нужно получить список уникальных точек. 
		std::vector<glm::vec2> uniquePoint;
		// сначала берем первую точку из начала первой стены
		uniquePoint.push_back(sector.walls[0].p1.pos);
		// все начальные точки стен равны концам предыдущих стен, поэтому берем только конечную точку p2
		for( size_t i = 0; i < sector.walls.size(); i++ )
		{
			if( sector.walls[i].p2.pos == uniquePoint[0] ) break;// но проверяем что конечная точка не равна самой первой (замыкая полигон)
			uniquePoint.push_back(sector.walls[i].p2.pos);
		}

		// триангуляция
		TPPLPoly poly;
		poly.Init((long)uniquePoint.size());
		for( size_t i = 0; i < uniquePoint.size(); i++ )
		{
			poly[i] = TPPLPoint{ uniquePoint[i].x, uniquePoint[i].y };
		}
		poly.SetOrientation(TPPL_ORIENTATION_CCW);

		TPPLPartition tri;
		int ret = tri.Triangulate_OPT(&poly, &sector.trianglesList);
		if( ret == 0 )
		{
			//poly.Invert();
			poly.SetOrientation(TPPL_ORIENTATION_CW);
			ret = tri.Triangulate_OPT(&poly, &sector.trianglesList);
		}
		assert(ret);
	}
}
//-----------------------------------------------------------------------------