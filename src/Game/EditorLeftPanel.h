#pragma once

#include "EditorLeftDrawHelper.h"
#include "EditorLeftGrid.h"
#include "EditorLeftCursor.h"
#include "EditorLeftViewport.h"
#include "EditorLeftCommand.h"
#include "EditorLeftMap.h"

class EditorLeftPanel
{
public:
	bool Create();
	void Destroy();
	void Update(float deltaTime);
	void Draw(float deltaTime);

	void SetActive(bool active);

	// курсор тут?
	bool IsMouseIn() const;

private:
	EditorLeftDrawHelper m_drawHelper;







	EditorLeftViewport m_leftViewport;
	EditorLeftGrid m_grid;
	EditorLeftCursor m_cursor;
	EditorLeftCommand m_commands;
	EditorLeftMap m_map;
	bool m_isActive = false;
};