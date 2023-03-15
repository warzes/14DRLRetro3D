#pragma once

#include "Engine.h"
#include "VertexFormat.h"

class EditorLeftViewport;

class EditorLeftGrid
{
public:
	bool Create();
	void Destroy();

	void Draw(const EditorLeftViewport& viewport);

	void GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const;

private:
	GeometryBuffer m_geomBuff;
	Texture2D m_gridTexture;
};