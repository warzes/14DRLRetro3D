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
	// ������ ����� �������

	if (app::IsMouseButtonPressed(0))
	{
		glm::vec2 posInMap = cursor.GetPosToMap();

		TempEditorVertexNum++;

		if (TempEditorVertexNum % 2 != 0) // ������ ����� �����
		{
			TempEditorCurrentWall.p1.pos = posInMap;
			TempEditorCurrentWall.p2.pos = glm::vec2(gridSize); // TODO: ����� ����������� �������������
		}
		else // ����� �����
		{
			TempEditorCurrentWall.p2.pos = posInMap;
			TempEditorWalls.push_back(TempEditorCurrentWall);

			TempEditorCurrentWall.p1.pos = TempEditorCurrentWall.p2.pos = glm::vec2(gridSize);
		}

		EditorDataChange = true;
	}
}
//-----------------------------------------------------------------------------