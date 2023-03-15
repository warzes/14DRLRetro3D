#include "stdafx.h"
#include "EditorLeftCommand.h"
#include "EditorLeftCursor.h"
#include "EditorSectorData.h"
#include "EditorLeftMap.h"
#include "Engine.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
void EditorLeftCommand::Update(const EditorLeftCursor& cursor, EditorLeftMap& map)
{
	// щелчок левой кнопкой

	if (app::IsMouseButtonPressed(0))
	{
		glm::vec2 posInMap = cursor.GetPosToMap();

		TempEditorVertexNum++;

		if (TempEditorVertexNum % 2 != 0) // первая точка стены
		{
			TempEditorCurrentWall.p1.pos = posInMap;
			TempEditorCurrentWall.p2.pos = glm::vec2(gridSize); // TODO: нужно присваивать бесконечность
		}
		else // конец стены
		{
			TempEditorCurrentWall.p2.pos = posInMap;
			TempEditorWalls.push_back(TempEditorCurrentWall);

			TempEditorCurrentWall.p1.pos = TempEditorCurrentWall.p2.pos = glm::vec2(gridSize);
		}

		EditorDataChange = true;
	}
}
//-----------------------------------------------------------------------------