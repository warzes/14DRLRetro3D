#include "stdafx.h"
#include "EditorLeftDrawHelper.h"
#include "EditorConstant.h"
#include "EditorLeftViewport.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::Create()
{
	if( !createGrid() )
		return false;

	if( !createSimpleShader() )
		return false;

	if( !createPoint() )
		return false;

	if( !createLine() )
		return false;

	if( !createCursor() )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::Destroy()
{
	destroyGrid();
	destroySimpleShader();
	destroyPoint();
	destroyLine();
	destroyCursor();
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::DrawGrid(const EditorLeftViewport& viewport) const
{
	assert(scene::IsValid(m_geomBufferGrid));

	render::Bind(m_textureGrid);
	render::Bind(m_shaderGrid);
	render::SetUniform(m_uniformGridWVP, viewport.GetOrthoProjection() * viewport.GetView() * glm::mat4(1.0f));
	render::Draw(m_geomBufferGrid.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::PreDrawPoint(const EditorLeftViewport& viewport, const glm::vec3& color) const
{
	assert(scene::IsValid(m_geomPoint));

	render::Bind(m_shaderSimpleColor);
	render::SetUniform(m_uniformSimpleColorDrawVP, viewport.GetOrthoProjection() * viewport.GetView());
	render::SetUniform(m_uniformSimpleColorDrawColor, color);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::DrawPoint(const glm::vec2& pos) const
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
	world = glm::scale(world, glm::vec3(1.2f));
	render::SetUniform(m_uniformSimpleColorDrawWorld, world);
	render::Draw(m_geomPoint.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::PreDrawLine(const EditorLeftViewport& viewport, const glm::vec3& color) const
{
	assert(scene::IsValid(m_geomLine));

	render::Bind(m_shaderLine);
	render::SetUniform(m_uniformGridViewProj, viewport.GetOrthoProjection() * viewport.GetView());
	render::SetUniform(m_uniformGridColor, color);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::DrawLine(const glm::vec2& pos1, const glm::vec2& pos2) const
{
	render::SetUniform(m_uniformGridPos, glm::vec4(pos1, pos2));
	render::Draw(m_geomLine.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::PreDrawCursor(const EditorLeftViewport& viewport, const glm::vec3& color) const
{
	assert(scene::IsValid(m_geomCursor));

	render::Bind(m_shaderSimpleColor);
	render::SetUniform(m_uniformSimpleColorDrawVP, viewport.GetOrthoProjection());
	render::SetUniform(m_uniformSimpleColorDrawColor, color);
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::DrawCursor(const glm::vec2& pos) const
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
	world = glm::scale(world, glm::vec3(3.5f));
	render::SetUniform(m_uniformSimpleColorDrawWorld, world);
	render::Draw(m_geomCursor.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::createGrid()
{
	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 uWVP;

out vec2 TexCoord;

void main()
{
	gl_Position = uWVP * vec4(vertexPosition, 0.0, 1.0);
	TexCoord = vertexTexCoord;
}
)";
	const char* fragmentShaderText = R"(
#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D Texture;

void main()
{
	outColor = texture(Texture, TexCoord);
}
)";

	m_shaderGrid = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	if( !render::IsValid(m_shaderGrid) )
		return false;
	m_uniformGridWVP = render::GetUniform(m_shaderGrid, "uWVP");

	// создается квад размером в зону карты - gridSize
	// в текстурных координатах ставится gridSize / gridStep чтобы создать эффект ячеек
	const VertexPos2Tex vert[] =
	{
		{{0.0f,               0.0f,            }, {0.0f,                               EditorMapGridSize / EditorGridStep}},
		{{0.0f,               EditorMapGridSize}, {0.0f,                               0.0f                              }},
		{{ EditorMapGridSize, EditorMapGridSize}, {EditorMapGridSize / EditorGridStep, 0.0f                              }},
		{{ EditorMapGridSize, 0.0f,            }, {EditorMapGridSize / EditorGridStep, EditorMapGridSize / EditorGridStep}}
	};
	constexpr uint16_t indexs[] = { 0, 1, 2, 2, 3, 0 };
	m_geomBufferGrid = scene::CreateGeometryBuffer(render::ResourceUsage::Static, 
		Countof(vert), sizeof(vert[0]), vert,
		Countof(indexs), sizeof(indexs[0]), indexs,
		m_shaderGrid);
	if( !scene::IsValid(m_geomBufferGrid) )
		return false;

	render::Texture2DInfo info;
	info.mipmap = false;
	info.magFilter = render::TextureMagFilter::Nearest;
	info.minFilter = render::TextureMinFilter::Nearest;
	info.wrapS = render::TextureWrapping::Repeat;
	info.wrapT = render::TextureWrapping::Repeat;

	m_textureGrid = render::CreateTexture2D("../data/textures/gridCell.png", true, info);
	if( !render::IsValid(m_textureGrid) )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::destroyGrid()
{
	scene::Destroy(m_geomBufferGrid);
	render::DestroyResource(m_textureGrid);
	render::DestroyResource(m_shaderGrid);
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::recreateGrid()
{
	destroyGrid();
	return createGrid();
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::createSimpleShader()
{
	const char* vertexShader = R"(
#version 330 core

layout(location = 0) in vec2 vertexPosition;

uniform mat4 uVP;
uniform mat4 uWorld;

void main()
{
	gl_Position = uVP * uWorld * vec4(vertexPosition, 0.0, 1.0);
}
)";
	const char* fragmentShader = R"(
#version 330 core

uniform vec3 uColor;

out vec4 outColor;

void main()
{
	outColor = vec4(uColor, 1.0);
}
)";

	m_shaderSimpleColor = render::CreateShaderProgram(vertexShader, fragmentShader);
	if( !render::IsValid(m_shaderSimpleColor) )
		return false;
	m_uniformSimpleColorDrawVP = render::GetUniform(m_shaderSimpleColor, "uVP");
	m_uniformSimpleColorDrawWorld = render::GetUniform(m_shaderSimpleColor, "uWorld");
	m_uniformSimpleColorDrawColor = render::GetUniform(m_shaderSimpleColor, "uColor");
	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::destroySimpleShader()
{
	render::DestroyResource(m_shaderSimpleColor);
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::createPoint()
{
	const VertexPos2 vert[] =
	{
		{{-0.5f, -0.5f}},
		{{-0.5f,  0.5f}},
		{{ 0.5f,  0.5f}},
		{{ 0.5f, -0.5f}}
	};

	const int indexs[] = { 0, 1, 2, 2, 3, 0 };

	m_geomPoint = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(vert[0]), vert, Countof(indexs), sizeof(indexs[0]), indexs, m_shaderSimpleColor);
	if( !scene::IsValid(m_geomPoint) )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::destroyPoint()
{
	scene::Destroy(m_geomPoint);
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::createLine()
{
	const char* vertexShader = R"(
#version 330 core

layout(location = 0) in float vertexId;

uniform mat4 uVP;
uniform vec4 uPos;

void main()
{
	gl_Position = uVP * vec4(((vertexId < 1.0) ? uPos.xy : uPos.zw), 0.0, 1.0);
}
)";
	const char* fragmentShader = R"(
#version 330 core

uniform vec3 uColor;
out vec4 outColor;

void main()
{
	outColor = vec4(uColor, 1.0);
}
)";

	m_shaderLine = render::CreateShaderProgram(vertexShader, fragmentShader);
	if( !render::IsValid(m_shaderLine) )
		return false;

	m_uniformGridViewProj = render::GetUniform(m_shaderLine, "uVP");
	m_uniformGridColor = render::GetUniform(m_shaderLine, "uColor");
	m_uniformGridPos = render::GetUniform(m_shaderLine, "uPos");

	const float vertData[] = { 0.0f, 1.0f };
	m_geomLine = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vertData), sizeof(vertData[0]), vertData, m_shaderLine);
	if( !scene::IsValid(m_geomLine) )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::destroyLine()
{
	render::DestroyResource(m_shaderLine);
	scene::Destroy(m_geomLine);
}
//-----------------------------------------------------------------------------
bool EditorLeftDrawHelper::createCursor()
{
	const VertexPos2 v[] =
	{
		{{-0.5f,  0.0f}},
		{{ 0.5f,  0.0f}},
		{{ 0.0f, -0.5f}},
		{{ 0.0f,  0.5f}},
	};

	m_geomCursor = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(v), sizeof(v[0]), v, m_shaderSimpleColor);
	if( !scene::IsValid(m_geomCursor) )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftDrawHelper::destroyCursor()
{
	scene::Destroy(m_geomCursor);
}
//-----------------------------------------------------------------------------