#include "stdafx.h"
#include "EditorLeftCursor.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
#include "EditorConstant.h"


возможность убирать 2д или 3д панели (то есть на все окно одну из панелей)

движение мышью
вершина кубом
при наведении мышки на вершину выделять стены цветом(обе)
при наведении мышки на стену - выделять ее цветом

при щелчке на вершину выдавать меню по настройке вершины
при щерчке на стену - меню
при щелчке на сектор - меню

у вершины в меню есть x и y координаты


возможные сущности
vertices
walls
sides
sectors
objects

идеи по редактору тут
https ://www.youtube.com/watch?v=fSjc8vLMg8c

bool EditorLeftCursor::Create()
{
	VertexPos3 v[] =
	{
		{{-0.5f,   0.0f, 0.0f}},
		{{ 0.5f,   0.0f, 0.0f}},
		{{ 0.0f,  -0.5f, 0.0f}},
		{{ 0.0f,   0.5f, 0.0f}},

	};

	m_geom = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(v), sizeof(VertexPos3), v, LineDrawShader);

	return true;
}

void EditorLeftCursor::Destroy()
{
	scene::Destroy(m_geom);
}

void EditorLeftCursor::Update(const glm::vec2& cam, const EditorLeftGrid& grid)
{
	glm::ivec2 posInMap;
	glm::vec2 offset;
	glm::vec2 sizeCell;
	grid.GetMouseInfo(cam, posInMap, offset, sizeCell);

	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
	m_pos = app::GetMousePosition();
	if( m_pos.x > halfScreenWidth ) m_pos.x = halfScreenWidth;

	const float sizeScaleX = halfScreenWidth / viewSize;
	const float sizeScaleY = app::GetWindowHeight() / viewSize;

	m_pos.x /= sizeScaleX;
	m_pos.y /= sizeScaleY;

	std::string text = "pos=" + std::to_string(m_pos.x) + "-" + std::to_string(m_pos.y);
	LogPrint(text);
} 

void EditorLeftCursor::Draw(const glm::vec2& cam)
{
	glm::mat4 proj = glm::ortho(0.0f, (float)viewSize, (float)viewSize, 0.0f, -1.0f, 1.0f);

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(m_pos.x, m_pos.y,0));
	world = glm::scale(world, glm::vec3(3.0f));

	render::Bind(LineDrawShader);
	render::SetUniform(UniformLineDrawProj, proj);
	render::SetUniform(UniformLineDrawView, glm::mat4(1.0f));
	render::SetUniform(UniformLineDrawWorld, world);
	render::SetUniform(UniformLineDrawColor, glm::vec3(1.0f, 0.8f, 0.2f));

	render::Draw(m_geom.vao, render::PrimitiveDraw::Lines);
}
