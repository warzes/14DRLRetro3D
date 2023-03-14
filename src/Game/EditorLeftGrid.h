#pragma once

#include "Engine.h"
#include "VertexFormat.h"

class EditorLeftGrid
{
public:
	bool Create();
	void Destroy();

	void Update();
	void Draw(const glm::vec2& cam);

private:
	GeometryBuffer m_geomBuff;
	std::vector<VertexPos3> m_tempVertexBiffer;
	unsigned m_curNumVB = 0;
};