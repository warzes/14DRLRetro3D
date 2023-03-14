#include "stdafx.h"
#include "GameStateEditor.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
GameStateEditor gGameStateEditor;
//-----------------------------------------------------------------------------
bool GameStateEditor::OnCreate()
{
	if (!m_leftPanel.Create())
		return false;

	if (!m_rightPanel.Create())
		return false;


	return true;
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnDestroy()
{
	m_leftPanel.Destroy();
	m_rightPanel.Destroy();
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnActive()
{
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnUpdate(float deltaTime)
{
	const ImGuiIO& io = ImGui::GetIO();

	if( !io.WantCaptureMouse )
	{
		const glm::vec2 realMousePos = app::GetMousePosition();
		const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;

		if( app::IsMouseButtonDown(0) )
		{
			if( realMousePos.x <= halfScreenWidth && !m_rightPanel.IsActive() )
			{
				m_leftPanel.SetActive(true);
				m_rightPanel.SetActive(false);
				app::SetMouseLock(false);
			}
			else
			{
				m_leftPanel.SetActive(false);
				m_rightPanel.SetActive(true);
				app::SetMouseLock(true);
			}
		}

		if( !io.WantCaptureKeyboard && app::IsKeyDown(app::KEY_ESCAPE) )
		{
			if( m_rightPanel.IsActive() )
			{
				m_leftPanel.SetActive(true);
				m_rightPanel.SetActive(false);
				app::SetMouseLock(false);
			}
		}

		m_leftPanel.Update(deltaTime);
		m_rightPanel.Update(deltaTime);
	}
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnFrame(float deltaTime)
{
	app::BeginFrame();
	
	m_leftPanel.Draw(deltaTime);
	//m_rightPanel.Draw(deltaTime);

	app::EndFrame();
}
//-----------------------------------------------------------------------------