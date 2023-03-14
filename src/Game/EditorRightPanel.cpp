#include "stdafx.h"
#include "EditorRightPanel.h"
#include "WorldRender.h"
#include "ShaderCode.h"

bool EditorRightPanel::Create()
{
	struct testVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	constexpr testVertex vert[] =
	{
		{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		{{-1.0f,  1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{{ 1.0f, -1.0f, 4.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	constexpr int indexs[] =
	{
		0, 1, 2,
		2, 3, 0
	};


	m_vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert);
	m_ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
	m_vao = render::CreateVertexArray(&m_vb, &m_ib, shader);

	render::Texture2DInfo texInfo;
	texInfo.mipmap = false;
	texInfo.minFilter = render::TextureMinFilter::NearestMipmapNearest;
	texInfo.magFilter = render::TextureMagFilter::Nearest;
	m_texture = render::CreateTexture2D("../data/textures/tile.png", true, texInfo);

	m_cam.position = { 0.0f, 0.0f, -10.0f };
	m_cam.viewPoint = { 0.0f, 0.0f, 1.0f };
	m_cam.upVector = { 0.0f, 1.0f, 0.0f };

	m_model = scene::CreateModel("../rock.obj", "../");

	CreateWorldRender();

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

	const float aspectRatio = (float)halfScreenWidth / (float)app::GetWindowHeight();
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);
	glm::mat4 view = scene::GetCameraViewMatrix(m_cam);

	render::Bind(shader);
	render::SetUniform(uniformProj, proj);
	render::SetUniform(uniformView, view);

	render::Bind(m_texture);
	render::Draw(m_vao);

	DrawWorldRender(m_cam);

	render::Bind(shaderModel);
	render::SetUniform(uniformModelViewMatrix, view);
	render::SetUniform(uniformModelProjectionMatrix, proj);
	render::SetUniform(uniformModelWorldMatrix, glm::mat4(1.0f));
	scene::Draw(m_model);
}

void EditorRightPanel::SetActive(bool active)
{
	m_isActive = active;
}

bool EditorRightPanel::IsActive() const
{
	return m_isActive;
}