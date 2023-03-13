#pragma once

#include "Engine.h"

class EditorRightPanel
{
public:
	bool Create();
	void Destroy();
	void Update(float deltaTime);
	void Draw(float deltaTime);

	void SetActive(bool active);
	bool IsActive() const;

private:
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_vao;
	Texture2D m_texture;
	scene::Camera m_cam;

	Model m_model;
	bool m_isActive = false;
};