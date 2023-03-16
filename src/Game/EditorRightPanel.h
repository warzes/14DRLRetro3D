#pragma once

#include "Engine.h"

struct SectorEditorWall;

class EditorRightPanel
{
public:
	bool Create();
	void Destroy();
	void Update(float deltaTime);
	void Draw(float deltaTime);

	void SetActive(bool active);
	bool IsActive() const;

private:
	void drawSectors();
	//VertexBuffer m_vb;
	//IndexBuffer m_ib;
	//VertexArray m_vao;
	Texture2D m_texture;
	scene::Camera m_cam;

	GeometryBuffer geom;


	//Model m_model;
	bool m_isActive = false;
};