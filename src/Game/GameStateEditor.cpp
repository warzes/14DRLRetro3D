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

	m_leftPanel.SetActive(true);

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
	// переключение между левой и правой панелями редактора
	const ImGuiIO& io = ImGui::GetIO();
	if( !io.WantCaptureMouse )
	{
		const glm::vec2 realMousePos = app::GetMousePosition();

		if( app::IsMouseButtonDown(0) )
		{
			if( realMousePos.x <= m_leftPanel.GetLeftViewportInfo().GetWidth() && !m_rightPanel.IsActive() )
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
	m_rightPanel.Draw(deltaTime);

	app::EndFrame();
}
//-----------------------------------------------------------------------------