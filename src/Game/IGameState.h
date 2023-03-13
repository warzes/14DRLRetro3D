#pragma once

class GameStateManager;

class IGameState
{
	friend class GameStateManager;
public:
	// OnCreate - создание ресурсов
	virtual bool OnCreate() = 0;
	// OnDestroy - удаление ресурсов
	virtual void OnDestroy() = 0;

	// OnActive - если нужно что-то сбросить при переходе в стейт, например в меню
	virtual void OnActive() = 0;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnFrame(float deltaTime) = 0;

private:
	bool create(GameStateManager* manager);
	void destroy();

	GameStateManager* m_manager = nullptr;
	bool m_isCreate = false;
};