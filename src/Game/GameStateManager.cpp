#include "stdafx.h"
#include "GameStateManager.h"

void GameStateManager::Destroy()
{
	for( auto& it : m_gameState )
	{
		it->destroy();
	}
	m_gameState.clear();
}

void GameStateManager::AddState(IGameState* state)
{
	m_gameState.push_back(state);
}

bool GameStateManager::SetActiveState(IGameState* state)
{
	if( !state->create(this) )
		return false;

	state->OnActive();
	m_currentState = state;
	return true;
}

void GameStateManager::Update(float deltaTime)
{
	if( m_currentState ) m_currentState->OnUpdate(deltaTime);
}

void GameStateManager::Frame(float deltaTime)
{
	if( m_currentState ) m_currentState->OnFrame(deltaTime);
}