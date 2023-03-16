#include "stdafx.h"
#include "EditorRightPanel.h"
#include "WorldRender.h"
#include "ShaderCode.h"
#include "EditorSectorData.h"
#include "VertexFormat.h"

bool EditorRightPanel::Create()
{
	//struct testVertex
	//{
	//	glm::vec3 pos;
	//	glm::vec3 color;
	//	glm::vec2 texCoord;
	//};

	//constexpr testVertex vert[] =
	//{
	//	{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
	//	{{-1.0f,  1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	//	{{ 1.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	//	{{ 1.0f, -1.0f, 4.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	//};

	//constexpr int indexs[] =
	//{
	//	0, 1, 2,
	//	2, 3, 0
	//};


	//m_vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert);
	//m_ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
	//m_vao = render::CreateVertexArray(&m_vb, &m_ib, shader);

	render::Texture2DInfo texInfo;
	texInfo.mipmap = false;
	texInfo.minFilter = render::TextureMinFilter::NearestMipmapNearest;
	texInfo.magFilter = render::TextureMagFilter::Nearest;
	texInfo.wrapS = render::TextureWrapping::Repeat;
	texInfo.wrapT = render::TextureWrapping::Repeat;
	m_texture = render::CreateTexture2D("../data/textures/tile.png", true, texInfo);

	m_cam.position = { 0.0f, 0.0f, -10.0f };
	m_cam.viewPoint = { 0.0f, 0.0f, 1.0f };
	m_cam.upVector = { 0.0f, 1.0f, 0.0f };

	/*m_model = scene::CreateModel("../rock.obj", "../");

	CreateWorldRender();*/

	return true;
}

void EditorRightPanel::Destroy()
{
}

void EditorRightPanel::Update(float deltaTime)
{
	(void)deltaTime;
	if (!m_isActive) return;

	const float mouseSensitivity = 10.0f * app::GetDeltaTime();
	const float moveSpeed = 10.0f * app::GetDeltaTime();
	const glm::vec3 oldCameraPos = m_cam.position;

	if (app::IsKeyDown(app::KEY_W)) scene::CameraMoveBy(m_cam, moveSpeed);
	if (app::IsKeyDown(app::KEY_S)) scene::CameraMoveBy(m_cam, -moveSpeed);
	if (app::IsKeyDown(app::KEY_A)) scene::CameraStrafeBy(m_cam, moveSpeed);
	if (app::IsKeyDown(app::KEY_D)) scene::CameraStrafeBy(m_cam, -moveSpeed);

	glm::vec2 delta = app::GetMouseDelta();
	if (delta.x != 0.0f) scene::CameraRotateLeftRight(m_cam, delta.x * mouseSensitivity);
	if (delta.y != 0.0f) scene::CameraRotateUpDown(m_cam, -delta.y * mouseSensitivity);
}

void EditorRightPanel::Draw(float deltaTime)
{
	(void)deltaTime;
	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;

	glEnable(GL_DEPTH_TEST);
	glViewport((GLint)halfScreenWidth, 0, app::GetWindowHeight(), app::GetWindowHeight());
	glScissor((GLint)halfScreenWidth, 0, app::GetWindowHeight(), app::GetWindowHeight());

	const float aspectRatio = halfScreenWidth / (float)app::GetWindowHeight(); // TODO: проверить
	glm::mat4 mat = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);
	glm::vec3 LightDirection = -glm::normalize(m_cam.viewPoint - m_cam.position);
	//glm::vec3(0.0f, 0.5f, -1.0f);

	render::Bind(SectorRenderShader);
	render::SetUniform(SectorRenderUniformProj, mat);
	render::SetUniform(SectorRenderUniformView, scene::GetCameraViewMatrix(m_cam));

	render::SetUniform(SectorRenderUniformLightAmbient, 0.333333f);
	render::SetUniform(SectorRenderUniformLightDiffuse, 0.666666f);
	render::SetUniform(SectorRenderUniformLightDirection, LightDirection);

	render::Bind(m_texture);
	drawSectors();
	//const float aspectRatio = (float)halfScreenWidth / (float)app::GetWindowHeight();
	//glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);
	//glm::mat4 view = scene::GetCameraViewMatrix(m_cam);

	//render::Bind(shader);
	//render::SetUniform(uniformProj, proj);
	//render::SetUniform(uniformView, view);

	//render::Bind(m_texture);
	//render::Draw(m_vao);

	//DrawWorldRender(m_cam);

	//render::Bind(shaderModel);
	//render::SetUniform(uniformModelViewMatrix, view);
	//render::SetUniform(uniformModelProjectionMatrix, proj);
	//render::SetUniform(uniformModelWorldMatrix, glm::mat4(1.0f));
	//scene::Draw(m_model);
}

void EditorRightPanel::SetActive(bool active)
{
	m_isActive = active;
}

bool EditorRightPanel::IsActive() const
{
	return m_isActive;
}

void EditorRightPanel::drawSectors()
{
	for( size_t i = 0; i < TempEditorSectors.size(); i++ )
	{
		auto& sector = TempEditorSectors[i];
		assert(sector.walls.size() > 0);

		// draw wall
		{
			std::vector<WorldVertex> data;
			for( int i = 0; i < sector.walls.size(); i++ )
			{
				const auto& w = sector.walls[i];

				float FloorHeight = sector.FloorHeight;
				float CeilingHeight = sector.CeilingHeight;

	/*			if( w.portal > 0 )
				{
					const unsigned idSector = w.portal - 1;

					if( idSector < sectors.size() && currentId != idSector )
					{
						DrawSectors(idSector, sectors, textures);
						if( sector.FloorHeight >= sectors[idSector].FloorHeight ) continue;
						else
						{
							CeilingHeight = sectors[idSector].FloorHeight;
						}
					}
				}*/

				float sizeWallX = glm::max(w.p1.pos.x, w.p2.pos.x) - glm::min(w.p1.pos.x, w.p2.pos.x);
				if( sizeWallX <= 0.0f )
					sizeWallX = glm::max(w.p1.pos.y, w.p2.pos.y) - glm::min(w.p1.pos.y, w.p2.pos.y);

				const float sizeWallY = CeilingHeight - FloorHeight;

				const glm::vec3 v1 = { w.p1.pos.x, FloorHeight,   w.p1.pos.y };
				const glm::vec3 v2 = { w.p1.pos.x, CeilingHeight, w.p1.pos.y };
				const glm::vec3 v3 = { w.p2.pos.x, FloorHeight,   w.p2.pos.y };
				const glm::vec3 v4 = { w.p1.pos.x, CeilingHeight, w.p1.pos.y };
				const glm::vec3 v5 = { w.p2.pos.x, CeilingHeight, w.p2.pos.y };
				const glm::vec3 v6 = { w.p2.pos.x, FloorHeight,   w.p2.pos.y };

				const glm::vec3 normal1 = glm::triangleNormal(v1, v2, v3);
				const glm::vec3 normal2 = glm::triangleNormal(v4, v5, v6);

				const glm::vec2 t1 = { 0.0f,             0.0f };
				const glm::vec2 t2 = { 0.0f,             1.0f * sizeWallY };
				const glm::vec2 t3 = { 1.0f * sizeWallX, 0.0f };
				const glm::vec2 t4 = { 1.0f * sizeWallX, 1.0f * sizeWallY };

				data.push_back({ v1, normal1, {1.0f, 1.0f, 1.0f}, t2 });
				data.push_back({ v2, normal1, {1.0f, 1.0f, 1.0f}, t1 });
				data.push_back({ v3, normal1, {1.0f, 1.0f, 1.0f}, t4 });
				data.push_back({ v4, normal2, {1.0f, 1.0f, 1.0f}, t1 });
				data.push_back({ v5, normal2, {1.0f, 1.0f, 1.0f}, t3 });
				data.push_back({ v6, normal2, {1.0f, 1.0f, 1.0f}, t4 });
			}
			render::UpdateVertexBuffer(sector.wall.vb, 0, data.size(), sizeof(WorldVertex), data.data());
			//render::Bind(textures[sector.walls[0].textureId]); // TODO: сделать текстуры разным стенам
			render::Draw(sector.wall.vao);
		}

		// draw floor and ceiling
		{
			std::vector<WorldVertex> data;
			for( auto it = sector.trianglesList.begin(); it != sector.trianglesList.end(); ++it )
			{
				const auto& t = *it;
				assert(t.GetNumPoints() == 3);
				for( int i = 0; i < t.GetNumPoints(); i++ )
				{
					glm::vec3 v = { t[i].x, sector.FloorHeight, t[i].y };
					glm::vec2 t = { sector.max.x - v.x, sector.max.y - v.z };
					data.push_back({ v, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, t });
				}
			}

			// рисуем пол
			render::UpdateVertexBuffer(sector.floor.vb, 0, data.size(), sizeof(WorldVertex), data.data());
			//render::Bind(textures[sector.FloorTextureId]);
			render::Draw(sector.floor.vao);

			// все тоже самое что и у пола, только изменена высота и нормаль
			for( size_t i = 0; i < data.size(); i++ )
			{
				data[i].pos.y = sector.CeilingHeight;
				data[i].normal.y = -1.0f;
			}
			// рисуем потолок
			render::UpdateVertexBuffer(sector.ceilling.vb, 0, data.size(), sizeof(WorldVertex), data.data());
			//render::Bind(textures[sector.CeilingTextureId]);
			render::Draw(sector.ceilling.vao);
		}
	}
}