#pragma once

#include "IGameState.h"
#include "Engine.h"

class GameStateTest final : public IGameState
{
public:
	bool OnCreate();
	void OnDestroy();
	void OnActive();

	void OnUpdate(float deltaTime);
	void OnFrame(float deltaTime);

private:
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_vao;
	Texture2D m_texture;
	scene::Camera m_cam;

	Model m_model;
};

extern GameStateTest gGameStateTest;