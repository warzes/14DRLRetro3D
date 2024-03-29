#include "stdafx.h"
#include "EditorLeftPanel.h"
#include "Engine.h"
//-----------------------------------------------------------------------------
bool EditorLeftPanel::Create()
{
	if( !m_drawHelper.Create() )
	{
		Fatal("Editor Draw Helper Init failed!");
		return false;
	}		
	if( !m_map.Create() )
	{
		Fatal("Editor Map Init failed!");
		return false;
	}

	if( !m_cursor.Create() )
	{
		Fatal("Editor Cursor Manager Init failed!");
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Destroy()
{
	m_cursor.Destroy();
	m_map.Destroy();
	m_drawHelper.Destroy();
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Update(float deltaTime)
{
	if (!m_isActive) return;

	m_leftViewport.Update(deltaTime);
	m_cursor.Update(m_leftViewport);
	m_commands.Update(m_cursor, m_map);
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Draw()
{
	m_leftViewport.SetOpenGLViewport();
	m_map.Draw(m_leftViewport, m_drawHelper);
	m_cursor.Draw(m_leftViewport, m_drawHelper);
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