#pragma once

#include "Engine.h"
#include "EditorLeftGrid.h"

class EditorLeftCursor
{
public:
	bool Create();
	void Destroy();

	void Update(const glm::vec2& cam, const EditorLeftGrid& grid);
	void Draw(const glm::vec2& cam);

private:
	GeometryBuffer m_geom;
	glm::vec2 m_pos;
};