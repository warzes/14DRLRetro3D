#pragma once

#include "IGameState.h"

class GameStateManager
{
public:
	void Destroy();

	void AddState(IGameState* state);

	bool SetActiveState(IGameState* state);

	void Update(float deltaTime);
	void Frame(float deltaTime);

private:
	std::vector<IGameState*> m_gameState;
	IGameState* m_currentState = nullptr;
};