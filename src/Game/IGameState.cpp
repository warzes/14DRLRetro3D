#include "stdafx.h"
#include "IGameState.h"
//-----------------------------------------------------------------------------
bool IGameState::create(GameStateManager* manager)
{
	m_manager = manager;
	if( !m_isCreate )
	{
		if( !OnCreate() ) return false;
	}
	m_isCreate = true;
	return true;
}
//-----------------------------------------------------------------------------
void IGameState::destroy()
{
	OnDestroy();
	m_isCreate = false;
}
//-----------------------------------------------------------------------------