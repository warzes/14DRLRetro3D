#pragma once

#include "Engine.h"

class EditorLeftViewport;

class EditorLeftMap
{
public:
	bool Create();
	void Destroy();

	void Draw(const EditorLeftViewport& viewport) const;

private:
	void drawPoint(const glm::vec2& pos) const;
	void drawLine(const glm::vec2& pos1, const glm::vec2& pos2) const;
	GeometryBuffer m_geomPoint;
	GeometryBuffer m_geomWall;
};