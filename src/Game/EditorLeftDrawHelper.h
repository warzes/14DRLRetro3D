#pragma once

#include "Engine.h"

class EditorLeftViewport;

class EditorLeftDrawHelper
{
public:
	bool Create();
	void Destroy();

	void DrawGrid(const EditorLeftViewport& viewport) const;

	void PreDrawPoint(const EditorLeftViewport& viewport, const glm::vec3& color) const;
	void DrawPoint(const glm::vec2& pos) const;

	void PreDrawLine(const EditorLeftViewport& viewport, const glm::vec3& color) const;
	void DrawLine(const glm::vec2& pos1, const glm::vec2& pos2) const;

	void PreDrawCursor(const EditorLeftViewport& viewport, const glm::vec3& color) const;
	void DrawCursor(const glm::vec2& pos) const;

private:
	bool createGrid();
	void destroyGrid();
	bool recreateGrid();

	bool createSimpleShader();
	void destroySimpleShader();

	bool createPoint();
	void destroyPoint();

	bool createLine();
	void destroyLine();

	bool createCursor();
	void destroyCursor();

	// Grid data
	GeometryBuffer m_geomBufferGrid;
	Texture2D m_textureGrid;
	ShaderProgram m_shaderGrid;
	Uniform m_uniformGridWVP;

	// Point
	GeometryBuffer m_geomPoint;

	// Simple shader
	ShaderProgram m_shaderSimpleColor;
	Uniform m_uniformSimpleColorDrawVP;
	Uniform m_uniformSimpleColorDrawWorld;
	Uniform m_uniformSimpleColorDrawColor;

	// Line
	GeometryBuffer m_geomLine;
	ShaderProgram m_shaderLine;
	Uniform m_uniformGridViewProj;
	Uniform m_uniformGridColor;
	Uniform m_uniformGridPos;

	// Cursor
	GeometryBuffer m_geomCursor;
};