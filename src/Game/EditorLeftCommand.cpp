#include "stdafx.h"
#include "EditorLeftCommand.h"
#include "EditorLeftCursor.h"
#include "EditorSectorData.h"
#include "EditorLeftMap.h"
#include "Engine.h"
#include "EditorConstant.h"
#include "ShaderCode.h"
#include "VertexFormat.h"
#include "Collisions.h"
//-----------------------------------------------------------------------------
void EditorLeftCommand::Update(const EditorLeftCursor& cursor, EditorLeftMap& map)
{
	// при движении мыши
	const glm::vec2 posInMap = cursor.PosToMap();
	CurrentCursorPoint.pos = posInMap;
	ProbableWallColor = { 0.4f, 0.4f, 1.0f };
	if( !checkCursorPoint() && (CurrentCursorPoint.pos != TempEditorVertices[0].pos || TempEditorVertices.size() < 3) )
		ProbableWallColor = { 1.4f, 0.0f, 0.0f };

	// клик левой кнопкой мыши
	if( app::IsMouseButtonReleased(0) )
	{
		bool isAddPoint = false;
		isAddPoint = addPoint();
	}

	if( app::IsKeyPressed(app::KEY_BACKSPACE) )
	{
		// удалить последнюю вершину временного сектора
		if ( TempEditorVertices.size() > 0 )
			TempEditorVertices.pop_back();
	}
}
//-----------------------------------------------------------------------------
bool EditorLeftCommand::checkCursorPoint() const
{
	if( !CurrentCursorPoint.IsValid() ) // точка невалидна
		return false;

	// если сейчас идет создание нового сектора, то нельзя щелкать по имеющимся точкам этого сектора (по другим можно)
	if( IsContains(TempEditorVertices, CurrentCursorPoint) ) return false;

	// проверка на пересечение стен нового сектора
	if( TempEditorVertices.size() > 1 )
	{
		const glm::vec2 p1 = TempEditorVertices[TempEditorVertices.size() - 1].pos;
		const glm::vec2 p2 = CurrentCursorPoint.pos;
		glm::vec2 nouse;
		for( size_t i = 1; i < TempEditorVertices.size()-1; i++ )
		{
			const glm::vec2 p3 = TempEditorVertices[i-1].pos;
			const glm::vec2 p4 = TempEditorVertices[i].pos;

			if( Intersection2D(p1, p2, p3, p4, nouse) )
				return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool EditorLeftCommand::addPoint()
{
	// сектор замыкается?
	if( EditorNewSector && TempEditorVertices.size() > 2 && CurrentCursorPoint == TempEditorVertices[0] )
	{
		buildEditorSector();
		return false;
	}

	if( !checkCursorPoint() ) return false;

	// ошибок не было, значит добавляем новую точку
	TempEditorVertices.push_back(CurrentCursorPoint);
	EditorDataChange = true;
	EditorNewSector = true;
	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftCommand::buildEditorSector()
{
	assert(TempEditorVertices.size() >= 3);
	EditorDataChange = true;
	EditorNewSector = false;

	SectorEditorSector newSector;
	for( size_t i = 1; i < TempEditorVertices.size(); i++ )
	{
		assert(TempEditorVertices[i].IsValid());
		const SectorEditorWall wall = { 
			.p1 = TempEditorVertices[i - 1], 
			.p2 = TempEditorVertices[i] 
		};
		newSector.walls.push_back(wall);
	}
	// вставляем стену которая соединена с первой точкой
	const SectorEditorWall endWall = { 
		.p1 = TempEditorVertices[TempEditorVertices.size() - 1], 
		.p2 = TempEditorVertices[0] 
	};
	newSector.walls.push_back(endWall);
	newSector.Build();

	TempEditorSectors.push_back(newSector);
	TempEditorVertices.clear();
}
//-----------------------------------------------------------------------------