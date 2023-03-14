#include "stdafx.h"
#include "EditorLeftGrid.h"
#include "ShaderCode.h"
#include "EditorConstant.h"
//-----------------------------------------------------------------------------
bool EditorLeftGrid::Create()
{
	/*m_geomBuff = scene::CreateGeometryBuffer(render::ResourceUsage::Dynamic, 1, sizeof(VertexPos3), nullptr, LineDrawShader);
	m_tempVertexBiffer.resize(gridSize * 4);*/

	// создается квад размером в зону карты - gridSize=1000
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
	m_geomBuff2 = scene::CreateGeometryBuffer(render::ResourceUsage::Static, 4, sizeof(VertexPos3Tex), vert, 6, sizeof(int), indexs, GridCellShader);

	render::Texture2DInfo info;
	info.mipmap = false;
	info.magFilter = render::TextureMagFilter::Nearest;
	info.minFilter = render::TextureMinFilter::Nearest;
	info.wrapS = render::TextureWrapping::Repeat;
	info.wrapT = render::TextureWrapping::Repeat;

	m_tex = render::CreateTexture2D("../data/textures/gridCell.png", true, info);

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Destroy()
{
	//scene::Destroy(m_geomBuff);
	scene::Destroy(m_geomBuff2);
	render::DestroyResource(m_tex);
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Update(const glm::vec2& cam)
{
	glm::ivec2 pos;
	glm::vec2 offset;
	glm::vec2 sizeCell;
	GetMouseInfo(cam, pos, offset, sizeCell);
	glm::ivec2 offsetTest = glm::ivec2(0);
	if( offset.x > 0.5f ) offsetTest.x = 1;
	if( offset.y > 0.5f ) offsetTest.y = 1;

	glm::vec2 realMousePos = app::GetMousePosition();

	if( app::IsMouseButtonPressed(0) )
	{
		std::string text = "Mouse pos=" + std::to_string(pos.x) + ":" + std::to_string(pos.y);
		text += "- Offset=" + std::to_string(offset.x) + ":" + std::to_string(offset.y);
		text += "- Real=" + std::to_string(realMousePos.x) + ":" + std::to_string(realMousePos.y);
		LogPrint(text);
	}


	//m_curNumVB = 0;
	//for( size_t i = 0; i < gridSize; i += gridStep )
	//{
	//	m_tempVertexBiffer[m_curNumVB++] = { glm::vec3(i, 0.0f, 0.0f) };
	//	m_tempVertexBiffer[m_curNumVB++] = { glm::vec3(i, gridSize, 0.0f) };

	//	m_tempVertexBiffer[m_curNumVB++] = { glm::vec3(0.0f, i, 0.0f) };
	//	m_tempVertexBiffer[m_curNumVB++] = { glm::vec3(gridSize, i, 0.0f) };
	//}
	//render::UpdateVertexBuffer(m_geomBuff.vb, 0, m_curNumVB, sizeof(VertexPos3), m_tempVertexBiffer.data());
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::Draw(const glm::vec2& cam)
{
	glm::mat4 proj = glm::ortho(0.0f, (float)viewSize, (float)viewSize, 0.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cam.x, cam.y, -0.5f), glm::vec3(cam.x, cam.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//render::Bind(LineDrawShader);
	//render::SetUniform(UniformLineDrawProj, proj);
	//render::SetUniform(UniformLineDrawView, view);

	//render::SetUniform(UniformLineDrawWorld, glm::mat4(1.0f));
	//render::SetUniform(UniformLineDrawColor, glm::vec3(0.7f, 1.0f, 0.3f));

	//render::Draw(m_geomBuff.vao, render::PrimitiveDraw::Lines);

	render::Bind(m_tex);
	render::Bind(GridCellShader);
	render::SetUniform(UniformGridCellProj, proj);
	render::SetUniform(UniformGridCellView, view);
	render::SetUniform(UniformGridCellWorld, glm::mat4(1.0f));
	render::SetUniform(UniformGridCellColor, glm::vec3(0.7f, 1.0f, 0.3f));

	render::Draw(m_geomBuff2.vao, render::PrimitiveDraw::Triangles);
}
//-----------------------------------------------------------------------------
void EditorLeftGrid::GetMouseInfo(const glm::vec2& cam, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell)
{
	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
	const float sizeScaleX = halfScreenWidth / viewSize;
	const float sizeScaleY = app::GetWindowHeight() / viewSize;

	sizeCell.x = sizeScaleX * gridStep;
	sizeCell.y = sizeScaleY * gridStep;

	glm::vec2 realMousePos = app::GetMousePosition();

	const float posX = realMousePos.x / sizeCell.x + cam.x / gridStep;
	const float posY = realMousePos.y / sizeCell.y + cam.y / gridStep;

	posInMap.x = static_cast<int>(posX);
	posInMap.y = static_cast<int>(posY);

	offset.x = posX - (float)posInMap.x;
	offset.y = posY - (float)posInMap.y;
}
//-----------------------------------------------------------------------------