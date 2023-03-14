#pragma once

#include "Engine.h"
#include "VertexFormat.h"

class EditorLeftGrid
{
public:
	bool Create();
	void Destroy();

	void Update(const glm::vec2& cam);
	void Draw(const glm::vec2& cam);

	void GetMouseInfo(const glm::vec2& cam, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell) const;

private:
	GeometryBuffer m_geomBuff2;
	Texture2D m_tex;
	//GeometryBuffer m_geomBuff;
	//std::vector<VertexPos3> m_tempVertexBiffer;
	unsigned m_curNumVB = 0;
};