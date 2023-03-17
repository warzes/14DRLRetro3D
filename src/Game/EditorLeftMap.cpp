#include "stdafx.h"
#include "EditorLeftMap.h"
#include "EditorSectorData.h"
#include "EditorLeftDrawHelper.h"
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
		if( it.IsValid() ) drawer.DrawPoint(it.pos);
	}

	// draw a probable wall
	if( TempEditorVertices.size() > 0 && CurrentCursorPoint.IsValid())
	{
		drawer.PreDrawLine(viewport, CurrentCursorWallColor);
		drawer.DrawLine(TempEditorVertices[TempEditorVertices.size()-1].pos, CurrentCursorPoint.pos);
	}

	return;





	

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