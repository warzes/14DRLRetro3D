#include "stdafx.h"
#include "EditorLeftMap.h"
#include "EditorLeftViewport.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
#include "EditorLeftDrawHelper.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
//-----------------------------------------------------------------------------
bool EditorLeftMap::Create()
{
	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Destroy()
{
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const
{
	drawer.DrawGrid(viewport);

	// draw point
	drawer.PreDrawPoint(viewport, { 1.0f, 0.9f, 0.1f });
	for( auto& it : TempEditorVertices )
	{
		if ( it.IsValid()) drawer.DrawPoint(it.pos);
	}

	// draw line
	drawer.PreDrawLine(viewport, { 1.0f, 0.9f, 0.1f });
	for( size_t i = 0; i < TempEditorVertices.size(); i++ )
	{
		if( i + 1 < TempEditorVertices.size() )
		{
			drawer.DrawLine(TempEditorVertices[i].pos, TempEditorVertices[i + 1].pos);
		}
	}

	// draw sectors
	drawSectors(viewport, drawer);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawSectors(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const
{
	for( size_t i = 0; i < TempEditorSectors.size(); i++ )
	{
		for( size_t j = 0; j < TempEditorSectors[i].walls.size(); j++ )
		{
			drawer.PreDrawLine(viewport, { 0.8f, 0.9f, 0.7f });
			drawer.DrawLine(TempEditorSectors[i].walls[j].p1.pos, TempEditorSectors[i].walls[j].p2.pos);

			drawer.PreDrawPoint(viewport, { 0.8f, 0.9f, 0.7f });
			drawer.DrawPoint(TempEditorSectors[i].walls[j].p1.pos);
		}
	}
}
//-----------------------------------------------------------------------------
void EditorLeftMap::GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const
{
	glm::vec2 realCursorPos = app::GetMousePosition();
	realCursorPos.y = (viewport.GetHeight() - realCursorPos.y); // TODO: что-то с этим сделать

	if( realCursorPos.x < viewport.GetWidth() )
	{
		// TODO: перенести этот код в viewport или грид
		outPosToMap = viewport.PosToWorldSpace({ realCursorPos.x, realCursorPos.y, 0.0f }, true);
		const glm::ivec2 intPos = viewport.GetPosInMap({ realCursorPos.x, realCursorPos.y, 0.0f });
		//std::cout << "pos=" << std::to_string(m_pos.x) << ":" << std::to_string(m_pos.y);
		//std::cout << "    posMap=" << std::to_string(intPos.x) << ":" << std::to_string(intPos.y);

		const float offsetX = outPosToMap.x / EditorGridStep - intPos.x;
		const float offsetY = outPosToMap.y / EditorGridStep - intPos.y;

		outPosToMap = intPos * EditorGridStep;
		if( offsetX > 0.3f && offsetX < 0.7f ) outPosToMap.x += 0.5f * EditorGridStep;
		else if( offsetX > 0.6f ) outPosToMap.x += 1.0f * EditorGridStep;
		if( offsetY > 0.3f && offsetY < 0.7f ) outPosToMap.y += 0.5f * EditorGridStep;
		else if( offsetY > 0.6f ) outPosToMap.y += 1.0f * EditorGridStep;

		outPosToScreen.x = outPosToMap.x - viewport.GetCameraPosition().x;
		outPosToScreen.y = outPosToMap.y - viewport.GetCameraPosition().y;

		//std::cout << "    npos=" << std::to_string(offsetX) << ":" << std::to_string(offsetY) << std::endl;
	}
}
//-----------------------------------------------------------------------------