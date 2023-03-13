#pragma once

class GameStateManager;

class IGameState
{
	friend class GameStateManager;
public:
	// OnCreate - �������� ��������
	virtual bool OnCreate() = 0;
	// OnDestroy - �������� ��������
	virtual void OnDestroy() = 0;

	// OnActive - ���� ����� ���-�� �������� ��� �������� � �����, �������� � ����
	virtual void OnActive() = 0;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnFrame(float deltaTime) = 0;

private:
	bool create(GameStateManager* manager);
	void destroy();

	GameStateManager* m_manager = nullptr;
	bool m_isCreate = false;
};