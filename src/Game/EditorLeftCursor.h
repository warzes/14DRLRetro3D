#pragma once

#include "Engine.h"

class EditorLeftViewport;
class EditorLeftMap;

class EditorLeftCursor
{
public:
	bool Create();
	void Destroy();

	void Update(const EditorLeftViewport& viewport, const EditorLeftMap& map);
	void Draw(const EditorLeftViewport& viewport);

	const glm::vec2& GetPosToMap() const { return m_posToMap; }

private:
	GeometryBuffer m_geom;
	glm::vec2 m_posToMap;
	glm::vec2 m_posToScreen;
};