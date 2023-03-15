#pragma once

#include "Engine.h"

class EditorLeftViewport;

class EditorLeftCursor
{
public:
	bool Create();
	void Destroy();

	void Update(const EditorLeftViewport& viewport);
	void Draw(const EditorLeftViewport& viewport);

private:
	GeometryBuffer m_geom;
	glm::vec2 m_posToMap;
	glm::vec2 m_posToScreen;


};