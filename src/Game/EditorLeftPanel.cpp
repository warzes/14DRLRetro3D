#include "stdafx.h"
#include "EditorLeftPanel.h"
#include "Engine.h"
//-----------------------------------------------------------------------------
bool EditorLeftPanel::Create()
{
	if( !m_grid.Create() )
		return false;

	if( !m_cursor.Create() )
		return false;

	if ( !m_map.Create() )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Destroy()
{
	m_map.Destroy();
	m_cursor.Destroy();
	m_grid.Destroy();
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Update(float deltaTime)
{
	if (!m_isActive) return;

	m_leftViewport.Update(deltaTime);
	m_cursor.Update(m_leftViewport, m_grid);
	m_commands.Update(m_cursor, m_map);
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Draw(float deltaTime)
{
	(void)deltaTime;

	m_leftViewport.SetOpenGLViewport();
	m_grid.Draw(m_leftViewport);
	m_map.Draw(m_leftViewport);
	m_cursor.Draw(m_leftViewport);	
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::SetActive(bool active)
{
	m_isActive = active;
}
//-----------------------------------------------------------------------------
bool EditorLeftPanel::IsMouseIn() const
{
	const glm::vec2 realMousePos = app::GetMousePosition();
	return realMousePos.x <= m_leftViewport.GetWidth();
}
//-----------------------------------------------------------------------------