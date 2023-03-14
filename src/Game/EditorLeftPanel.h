#pragma once

#include "Engine.h"
#include "EditorLeftGrid.h"
#include "EditorLeftCamera.h"

class EditorLeftPanel
{
public:
	bool Create();
	void Destroy();
	void Update(float deltaTime);
	void Draw(float deltaTime);

	void SetActive(bool active);

private:
	void getMouseInfo(const glm::vec2& realPos, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell);

	EditorLeftGrid m_grid;
	EditorLeftCamera m_leftCamera;

	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_vao;
	bool m_isActive = false;
};