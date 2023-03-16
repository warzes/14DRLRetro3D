#include "stdafx.h"
#include "EditorLeftMap.h"
#include "EditorLeftViewport.h"
#include "EditorSectorData.h"
#include "EditorConstant.h"
#include "VertexFormat.h"
#include "ShaderCode.h"
//-----------------------------------------------------------------------------
bool EditorLeftMap::Create()
{
	// создается квад размером в зону карты - gridSize
	// в текстурных координатах ставится gridSize / gridStep чтобы создать эффект ячеек
	const VertexPos3Tex vert[] =
	{
		{{0.0f,      0.0f,     0.0f}, {0.0f,                EditorMapGridSize / EditorGridStep}},
		{{0.0f,      EditorMapGridSize, 0.0f}, {0.0f,                0.0f               }},
		{{ EditorMapGridSize, EditorMapGridSize, 0.0f}, {EditorMapGridSize / EditorGridStep, 0.0f               }},
		{{ EditorMapGridSize, 0.0f,     0.0f}, {EditorMapGridSize / EditorGridStep, EditorMapGridSize / EditorGridStep}}
	};
	constexpr int indexs[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	m_geomBuff = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(VertexPos3Tex), vert, Countof(indexs), sizeof(int), indexs, GridCellShader);
	if( !scene::IsValid(m_geomBuff) )
	{
		return false;
	}

	render::Texture2DInfo info;
	info.mipmap = false;
	info.magFilter = render::TextureMagFilter::Nearest;
	info.minFilter = render::TextureMinFilter::Nearest;
	info.wrapS = render::TextureWrapping::Repeat;
	info.wrapT = render::TextureWrapping::Repeat;

	m_gridTexture = render::CreateTexture2D("../data/textures/gridCell.png", true, info);
	if( !render::IsValid(m_gridTexture) )
	{
		return false;
	}





	constexpr VertexPos3 vert3[] =
	{
		{{-0.5f, -0.5f, .0f}},
		{{-0.5f,  0.5f, .0f}},
		{{ 0.5f,  0.5f, .0f}},
		{{ 0.5f, -0.5f, .0f}}
	};

	constexpr int indexs3[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	m_geomPoint = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vert3), sizeof(VertexPos3), vert3, Countof(indexs), sizeof(indexs3[0]), indexs3, SimpleColorShader);
	if (!scene::IsValid(m_geomPoint))
		return false;

	constexpr float vertData[] = { 0.0f, 1.0f };
	m_geomWall = scene::CreateGeometryBuffer(render::ResourceUsage::Static, Countof(vertData), sizeof(float), vertData, Simple2DLineDrawShader);
	if (!scene::IsValid(m_geomWall))
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Destroy()
{
	scene::Destroy(m_geomPoint);
	scene::Destroy(m_geomWall);

	scene::Destroy(m_geomBuff);
	render::DestroyResource(m_gridTexture);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::Draw(const EditorLeftViewport& viewport) const
{
	render::Bind(m_gridTexture);
	render::Bind(GridCellShader);
	render::SetUniform(UniformGridCellProj, viewport.GetOrthoProjection());
	render::SetUniform(UniformGridCellView, viewport.GetView());
	render::SetUniform(UniformGridCellWorld, glm::mat4(1.0f));
	render::SetUniform(UniformGridCellColor, glm::vec3(0.7f, 1.0f, 0.3f));

	assert(scene::IsValid(m_geomBuff));
	render::Draw(m_geomBuff.vao, render::PrimitiveDraw::Triangles);








	// draw point
	render::Bind(SimpleColorShader);
	render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
	render::SetUniform(SimpleColorDrawView, viewport.GetView());
	render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 0.9f, 0.1f));
	for( auto& it : TempEditorVertices )
	{
		if ( it.IsValid()) drawPoint(it.pos);
	}

	// draw line
	render::Bind(Simple2DLineDrawShader);
	render::SetUniform(Simple2DLineDrawViewProj, viewport.GetOrthoProjection() * viewport.GetView());
	render::SetUniform(Simple2DLineDrawColor, glm::vec3(1.0f, 0.9f, 0.1f));
	for( size_t i = 0; i < TempEditorVertices.size(); i++ )
	{
		if( i + 1 < TempEditorVertices.size() )
		{
			drawLine(TempEditorVertices[i].pos, TempEditorVertices[i+1].pos);
		}
	}

	// draw sectors
	drawSectors(viewport);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawPoint(const glm::vec2& pos) const
{
	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
	world = glm::scale(world, glm::vec3(1.2f));
	render::SetUniform(SimpleColorDrawWorld, world);
	render::Draw(m_geomPoint.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawLine(const glm::vec2& pos1, const glm::vec2& pos2) const
{
	render::SetUniform(Simple2DLineDrawPos, glm::vec4(pos1, pos2));
	render::Draw(m_geomWall.vao, render::PrimitiveDraw::Lines);
}
//-----------------------------------------------------------------------------
void EditorLeftMap::drawSectors(const EditorLeftViewport& viewport) const
{
	for( size_t i = 0; i < TempEditorSectors.size(); i++ )
	{
		for( size_t j = 0; j < TempEditorSectors[i].walls.size(); j++ )
		{
			// draw wall
			render::Bind(Simple2DLineDrawShader);
			render::SetUniform(Simple2DLineDrawViewProj, viewport.GetOrthoProjection() * viewport.GetView());
			render::SetUniform(Simple2DLineDrawColor, glm::vec3(0.8f, 0.9f, 0.7f));
			drawLine(TempEditorSectors[i].walls[j].p1.pos, TempEditorSectors[i].walls[j].p2.pos);

			render::Bind(SimpleColorShader);
			render::SetUniform(SimpleColorDrawProj, viewport.GetOrthoProjection());
			render::SetUniform(SimpleColorDrawView, viewport.GetView());
			render::SetUniform(SimpleColorDrawColor, glm::vec3(0.8f, 0.9f, 0.7f));
			drawPoint(TempEditorSectors[i].walls[j].p1.pos);
		}
	}
}
//-----------------------------------------------------------------------------
void EditorLeftMap::GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const
{
	glm::vec2 realCursorPos = app::GetMousePosition();
	realCursorPos.y = (viewport.GetHeight() - realCursorPos.y); // TODO: что-то с этим сделать

	if( realCursorPos.x < viewport.GetWidth() )
	{
		// TODO: перенести этот код в viewport или грид
		outPosToMap = viewport.PosToWorldSpace({ realCursorPos.x, realCursorPos.y, 0.0f }, true);
		const glm::ivec2 intPos = viewport.GetPosInMap({ realCursorPos.x, realCursorPos.y, 0.0f });
		//std::cout << "pos=" << std::to_string(m_pos.x) << ":" << std::to_string(m_pos.y);
		//std::cout << "    posMap=" << std::to_string(intPos.x) << ":" << std::to_string(intPos.y);

		const float offsetX = outPosToMap.x / EditorGridStep - intPos.x;
		const float offsetY = outPosToMap.y / EditorGridStep - intPos.y;

		outPosToMap = intPos * EditorGridStep;
		if( offsetX > 0.3f && offsetX < 0.7f ) outPosToMap.x += 0.5f * EditorGridStep;
		else if( offsetX > 0.6f ) outPosToMap.x += 1.0f * EditorGridStep;
		if( offsetY > 0.3f && offsetY < 0.7f ) outPosToMap.y += 0.5f * EditorGridStep;
		else if( offsetY > 0.6f ) outPosToMap.y += 1.0f * EditorGridStep;

		outPosToScreen.x = outPosToMap.x - viewport.GetCameraPosition().x;
		outPosToScreen.y = outPosToMap.y - viewport.GetCameraPosition().y;

		//std::cout << "    npos=" << std::to_string(offsetX) << ":" << std::to_string(offsetY) << std::endl;
	}
}
//-----------------------------------------------------------------------------