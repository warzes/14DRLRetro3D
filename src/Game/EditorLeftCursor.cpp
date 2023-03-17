#include "stdafx.h"
#include "EditorLeftCursor.h"
#include "EditorLeftMap.h"
#include "EditorLeftDrawHelper.h"
//-----------------------------------------------------------------------------
bool EditorLeftCursor::Create()
{
	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Destroy()
{
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::Update(const EditorLeftViewport& viewport, const EditorLeftMap& map)
{
	map.GetCursorToMap(viewport, m_posToMap, m_posToScreen);
} 
//-----------------------------------------------------------------------------
void EditorLeftCursor::Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer)
{
	drawer.PreDrawCursor(viewport, { 1.0f, 0.8f, 0.2f });
	drawer.DrawCursor(m_posToScreen);
}
//-----------------------------------------------------------------------------