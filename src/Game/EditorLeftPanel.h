#pragma once

#include "Engine.h"
#include "EditorLeftGrid.h"

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

	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_vao;
	glm::vec2 m_2dCam = glm::vec2(0.0f, 0.0f);
	bool m_isActive = false;
};