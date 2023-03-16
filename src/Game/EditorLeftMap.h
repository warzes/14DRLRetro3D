#pragma once

#include "Engine.h"

class EditorLeftViewport;

class EditorLeftMap
{
public:
	bool Create();
	void Destroy();

	void Draw(const EditorLeftViewport& viewport) const;

	void GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const;

private:
	void drawPoint(const glm::vec2& pos) const;
	void drawLine(const glm::vec2& pos1, const glm::vec2& pos2) const;
	void drawSectors(const EditorLeftViewport& viewport) const;
	GeometryBuffer m_geomPoint;
	GeometryBuffer m_geomWall;

	// TODO: rename grid
	GeometryBuffer m_geomBuff;
	Texture2D m_gridTexture;
};