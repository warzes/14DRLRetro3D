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





		TempEditorVertices.push_back({ posInMap });
		EditorDataChange = true;
	}
}
//-----------------------------------------------------------------------------