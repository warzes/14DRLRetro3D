#pragma once

#include "IGameState.h"
#include "EditorLeftPanel.h"
#include "EditorRightPanel.h"

class GameStateEditor final : public IGameState
{
public:
	bool OnCreate();
	void OnDestroy();
	void OnActive();

	void OnUpdate(float deltaTime);
	void OnFrame(float deltaTime);

private:
	bool createImgui();
	void destroyImgui();
	void activeLeftPanel();
	void activeRightPanel();

	EditorLeftPanel m_leftPanel;
	EditorRightPanel m_rightPanel;
};

extern GameStateEditor gGameStateEditor;