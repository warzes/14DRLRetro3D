#pragma once

#include "Engine.h"
#include "VertexFormat.h"

class EditorLeftViewport;

class EditorLeftGrid
{
public:
	bool Create();
	void Destroy();

	void Update(const EditorLeftViewport& viewport);
	void Draw(const EditorLeftViewport& viewport);

	//void GetMouseInfo(const EditorLeftViewport& viewport, glm::ivec2& outPosInMap, glm::vec2& outOffset, glm::vec2& outSizeCell) const;

private:
	GeometryBuffer m_geomBuff;
	Texture2D m_gridTexture;
};