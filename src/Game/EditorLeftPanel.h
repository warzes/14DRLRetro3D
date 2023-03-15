#pragma once

#include "Engine.h"
#include "EditorLeftGrid.h"
#include "EditorLeftCursor.h"
#include "EditorLeftViewport.h"

class EditorLeftPanel
{
public:
	bool Create();
	void Destroy();
	void Update(float deltaTime);
	void Draw(float deltaTime);

	void SetActive(bool active);

	const EditorLeftViewport& GetLeftViewportInfo() const { return m_leftViewport; }

private:
	void getMouseInfo(const glm::vec2& realPos, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell);

	EditorLeftViewport m_leftViewport;
	EditorLeftGrid m_grid;

	EditorLeftCursor m_cursor;
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_vao;
	bool m_isActive = false;
};