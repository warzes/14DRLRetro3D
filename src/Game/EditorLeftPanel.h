#pragma once

#include "EditorLeftDrawHelper.h"
#include "EditorLeftMap.h"


#include "EditorLeftCursor.h"
#include "EditorLeftViewport.h"
#include "EditorLeftCommand.h"


class EditorLeftPanel
{
public:
	bool Create();
	void Destroy();

	void Update(float deltaTime);
	void Draw();

	void SetActive(bool active);

	// курсор внутри панели
	bool IsMouseIn() const;

private:
	EditorLeftDrawHelper m_drawHelper;
	EditorLeftMap m_map;





	EditorLeftViewport m_leftViewport;

	EditorLeftCursor m_cursor;
	EditorLeftCommand m_commands;

	bool m_isActive = false;
};