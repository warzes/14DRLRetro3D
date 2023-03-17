#include "stdafx.h"
#include "EditorLeftCursor.h"
#include "EditorLeftDrawHelper.h"
#include "EditorLeftViewport.h"
#include "EditorConstant.h"
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
void EditorLeftCursor::Update(const EditorLeftViewport& viewport)
{
	getCursorToMap(viewport);
} 
//-----------------------------------------------------------------------------
void EditorLeftCursor::Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer)
{
	drawer.PreDrawCursor(viewport, { 1.0f, 0.8f, 0.2f });
	drawer.DrawCursor(m_posToScreen);
}
//-----------------------------------------------------------------------------
void EditorLeftCursor::getCursorToMap(const EditorLeftViewport& viewport)
{
	glm::vec2 realCursorPos = app::GetMousePosition();
	realCursorPos.y = (viewport.GetHeight() - realCursorPos.y);

	if( realCursorPos.x < viewport.GetWidth() )
	{
		m_posToMap = viewport.PosToWorldSpace({ realCursorPos.x, realCursorPos.y, 0.0f }, true);
		const glm::ivec2 intPos = viewport.GetPosInMap({ realCursorPos.x, realCursorPos.y, 0.0f });
		//std::cout << "pos=" << std::to_string(m_pos.x) << ":" << std::to_string(m_pos.y);
		//std::cout << "    posMap=" << std::to_string(intPos.x) << ":" << std::to_string(intPos.y);

		const float offsetX = m_posToMap.x / EditorGridStep - intPos.x;
		const float offsetY = m_posToMap.y / EditorGridStep - intPos.y;

		m_posToMap = intPos * EditorGridStep;
		if( offsetX > 0.3f && offsetX < 0.7f ) m_posToMap.x += 0.5f * EditorGridStep;
		else if( offsetX > 0.6f ) m_posToMap.x += 1.0f * EditorGridStep;
		if( offsetY > 0.3f && offsetY < 0.7f ) m_posToMap.y += 0.5f * EditorGridStep;
		else if( offsetY > 0.6f ) m_posToMap.y += 1.0f * EditorGridStep;

		m_posToScreen.x = m_posToMap.x - viewport.GetCameraPosition().x;
		m_posToScreen.y = m_posToMap.y - viewport.GetCameraPosition().y;
		//std::cout << "    npos=" << std::to_string(offsetX) << ":" << std::to_string(offsetY) << std::endl;
	}
}
//-----------------------------------------------------------------------------