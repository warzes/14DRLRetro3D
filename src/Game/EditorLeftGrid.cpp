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
void EditorLeftGrid::Update(const EditorLeftViewport& viewport)
{
	//glm::vec3 worldPos;
	//glm::ivec2 posInMap;
	//glm::vec2 sizeCell;
	//glm::vec2 offset;
	//viewport.GetCursorInfo(worldPos, posInMap, sizeCell, offset);

	//glm::ivec2 pos;
	//glm::vec2 offset;
	//glm::vec2 sizeCell;
	//GetMouseInfo(viewport, pos, offset, sizeCell);
	//glm::ivec2 offsetTest = glm::ivec2(0);
	//if( offset.x > 0.5f ) offsetTest.x = 1;
	//if( offset.y > 0.5f ) offsetTest.y = 1;

	//glm::vec2 realMousePos = app::GetMousePosition();

	//if( app::IsMouseButtonPressed(0) )
	//{
	//	std::string text = "Mouse pos=" + std::to_string(pos.x) + ":" + std::to_string(pos.y);
	//	text += "- Offset=" + std::to_string(offset.x) + ":" + std::to_string(offset.y);
	//	text += "- Real=" + std::to_string(realMousePos.x) + ":" + std::to_string(realMousePos.y);
	//	LogPrint(text);
	//}
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
////-----------------------------------------------------------------------------
//void EditorLeftGrid::GetMouseInfo(const EditorLeftViewport& viewport, glm::ivec2& outPosInMap, glm::vec2& outOffset, glm::vec2& outSizeCell) const
//{
//	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
//	const float sizeScaleX = halfScreenWidth / viewSize;
//	const float sizeScaleY = app::GetWindowHeight() / viewSize;
//
//	outSizeCell.x = sizeScaleX * gridStep;
//	outSizeCell.y = sizeScaleY * gridStep;
//
//	const glm::vec2 realMousePos = app::GetMousePosition();
//
//	const float posX = realMousePos.x / outSizeCell.x + viewport.GetCameraPosition().x / gridStep;
//	const float posY = realMousePos.y / outSizeCell.y + viewport.GetCameraPosition().y / gridStep;
//
//	outPosInMap.x = static_cast<int>(posX);
//	outPosInMap.y = static_cast<int>(posY);
//
//	outOffset.x = posX - (float)outPosInMap.x;
//	outOffset.y = posY - (float)outPosInMap.y;
//}
//-----------------------------------------------------------------------------