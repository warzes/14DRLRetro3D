#include "stdafx.h"
#include "EditorLeftCommand.h"
#include "EditorLeftCursor.h"
#include "EditorSectorData.h"
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

	// выбор точки
	if( app::IsMouseButtonPressed(0) )
	{
		bool isSelectPoint = selectPoint();
	}
	// клик (отпускание) левой кнопкой мыши
	if( app::IsMouseButtonReleased(0) )
	{
		// отмена выделения
		if( SelectPoint || SelectWall1 || SelectWall2 )
		{
			SelectPoint = nullptr;
			SelectWall1 = nullptr;
			SelectWall2 = nullptr;
		}
		else
		{
			bool isAddPoint = false;
			isAddPoint = addPoint();
		}
	}

	// движение точки
	bool isMovePoint = movePoint();

	if( app::IsKeyPressed(app::KEY_BACKSPACE) )
	{
		// удалить последнюю вершину временного сектора
		if( TempEditorVertices.size() > 0 )
		{
			TempEditorVertices.pop_back();
			SelectPoint = nullptr;
		}
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
		newSector.walls.push_back({
			.p1 = TempEditorVertices[i - 1],
			.p2 = TempEditorVertices[i]
			});
	}
	// вставляем стену которая соединена с первой точкой
	newSector.walls.push_back({
		.p1 = TempEditorVertices[TempEditorVertices.size() - 1],
		.p2 = TempEditorVertices[0]
		});
	newSector.Build();

	TempEditorSectors.push_back(newSector);
	TempEditorVertices.clear();
}
//-----------------------------------------------------------------------------
bool EditorLeftCommand::selectPoint()
{
	// точка временного сектора?
	for( size_t i = 1; i < TempEditorVertices.size(); i++ ) // TODO: пока нельзя двигать первую точку нового сектора, иначе не сработает замыкание сектора.
	{
		if( CurrentCursorPoint == TempEditorVertices[i] )
		{
			SelectPoint = &TempEditorVertices[i];
			SelectWall1 = SelectWall2 = nullptr;
			return true;
		}
	}

	if( TempEditorVertices.size() > 0 ) // нельзя выбирать точки других секторов, пока создаем новый
		return false;

	// точка с других секторов?
	for( size_t i = 0; i < TempEditorSectors.size(); i++ )
	{
		SectorEditorSector& sector = TempEditorSectors[i];
		// TODO: сделать проверку на аабб, не надо перебирать все стены, если сектор далеко от указателя

		for( size_t j = 0; j < sector.walls.size(); j++ )
		{
			SectorEditorWall& wall = sector.walls[j];

			if( wall.p1 == CurrentCursorPoint )
			{
				SelectWall1 = &wall;
				if( j > 0 ) SelectWall2 = &sector.walls[j - 1];
				else SelectWall2 = &sector.walls[sector.walls.size() - 1];
				SelectSector = &TempEditorSectors[i];
				return true;
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool EditorLeftCommand::movePoint()
{
	if( SelectPoint )
	{
		SelectPoint->pos = CurrentCursorPoint.pos;
		return true;
	}

	if( SelectWall1 && SelectWall2 )
	{
		SelectWall1->p1 = CurrentCursorPoint;
		SelectWall2->p2 = CurrentCursorPoint;
		SelectSector->ReBuild();
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------