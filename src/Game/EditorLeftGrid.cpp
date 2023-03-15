#include "stdafx.h"
#include "EditorLeftGrid.h"
#include "ShaderCode.h"
#include "EditorConstant.h"
#include "EditorLeftViewport.h"
//-----------------------------------------------------------------------------
bool EditorLeftGrid::Create()
{
	// создается квад размером в зону карты - gridSize
	// в текстурных координатах ставится gridSize / gridStep чтобы создать эффект ячеек
	constexpr VertexPos3Tex vert[] =
	{
		{{0.0f,      0.0f,     0.0f}, {0.0f,                gridSize / gridStep}},
		{{0.0f,      gridSize, 0.0f}, {0.0f,                0.0f               }},
		{{ gridSize, gridSize, 0.0f}, {gridSize / gridStep, 0.0f               }},
		{{ gridSize, 0.0f,     0.0f}, {gridSize / gridStep, gridSize / gridStep}}
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

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Destroy()
{
	scene::Destroy(m_geomBuff);
	render::DestroyResource(m_gridTexture);
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Draw(const EditorLeftViewport& viewport)
{
	render::Bind(m_gridTexture);
	render::Bind(GridCellShader);
	render::SetUniform(UniformGridCellProj, viewport.GetOrthoProjection());
	render::SetUniform(UniformGridCellView, viewport.GetView());
	render::SetUniform(UniformGridCellWorld, glm::mat4(1.0f));
	render::SetUniform(UniformGridCellColor, glm::vec3(0.7f, 1.0f, 0.3f));

	assert(scene::IsValid(m_geomBuff));
	render::Draw(m_geomBuff.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const
{
	glm::vec2 realCursorPos = app::GetMousePosition();
	realCursorPos.y = (viewport.GetHeight() - realCursorPos.y); // TODO: что-то с этим сделать

	if (realCursorPos.x < viewport.GetWidth())
	{
		// TODO: перенести этот код в viewport или грид
		outPosToMap = viewport.PosToWorldSpace({ realCursorPos.x, realCursorPos.y, 0.0f }, true);
		const glm::ivec2 intPos = viewport.GetPosInMap({ realCursorPos.x, realCursorPos.y, 0.0f });
		//std::cout << "pos=" << std::to_string(m_pos.x) << ":" << std::to_string(m_pos.y);
		//std::cout << "    posMap=" << std::to_string(intPos.x) << ":" << std::to_string(intPos.y);

		const float offsetX = outPosToMap.x / gridStep - intPos.x;
		const float offsetY = outPosToMap.y / gridStep - intPos.y;

		outPosToMap = intPos * gridStep;
		if (offsetX > 0.3f && offsetX < 0.7f) outPosToMap.x += 0.5f * gridStep;
		else if (offsetX > 0.6f) outPosToMap.x += 1.0f * gridStep;
		if (offsetY > 0.3f && offsetY < 0.7f) outPosToMap.y += 0.5f * gridStep;
		else if (offsetY > 0.6f) outPosToMap.y += 1.0f * gridStep;

		outPosToScreen.x = outPosToMap.x - viewport.GetCameraPosition().x;
		outPosToScreen.y = outPosToMap.y - viewport.GetCameraPosition().y;

		//std::cout << "    npos=" << std::to_string(offsetX) << ":" << std::to_string(offsetY) << std::endl;
	}
}
//-----------------------------------------------------------------------------