#include "stdafx.h"
#include "EditorLeftPanel.h"
#include "EditorConstant.h"
#include "Engine.h"

struct testPoint
{
	glm::vec2 pos;
	glm::vec2 realPos;
};

std::vector<testPoint> testPoints;

testPoint Cursor;

bool EditorLeftPanel::Create()
{
	constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(vertexPosition, 1.0);
}
)";
	constexpr const char* fragmentShaderText = R"(
#version 330 core

uniform vec3 uColor;

out vec4 outColor;

void main()
{
	outColor = vec4(uColor, 1.0);
}
)";


	m_shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	m_uniformProj = render::GetUniform(m_shader, "uProjection");
	m_uniformView = render::GetUniform(m_shader, "uView");
	m_uniformWorld = render::GetUniform(m_shader, "uWorld");
	m_uniformColor = render::GetUniform(m_shader, "uColor");

	struct lineVertex
	{
		glm::vec3 pos;
	};

	constexpr lineVertex vert[] =
	{
		{{-500.0f, -500.0f, 0.0f}},
		{{-500.0f,  500.0f, 0.0f}},
		{{ 500.0f,  500.0f, 0.0f}},
		{{ 500.0f, -500.0f, 0.0f}}
	};

	constexpr int indexs[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	m_vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(lineVertex), vert);
	m_ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
	m_vao = render::CreateVertexArray(&m_vb, &m_ib, m_shader);
	return true;
}

void EditorLeftPanel::Destroy()
{
	render::DestroyResource(m_shader);
	render::DestroyResource(m_vb);
	render::DestroyResource(m_ib);
	render::DestroyResource(m_vao);
}

void EditorLeftPanel::Update(float deltaTime)
{
	if (!m_isActive) return;

	if (app::IsKeyDown(app::KEY_A)) m_2dCam.x -= CameraSpeedInEditorLeftPanel * deltaTime;
	if (app::IsKeyDown(app::KEY_D)) m_2dCam.x += CameraSpeedInEditorLeftPanel * deltaTime;
	if (app::IsKeyDown(app::KEY_W)) m_2dCam.y -= CameraSpeedInEditorLeftPanel * deltaTime;
	if (app::IsKeyDown(app::KEY_S)) m_2dCam.y += CameraSpeedInEditorLeftPanel * deltaTime;
	if (m_2dCam.x < 0.0f) m_2dCam.x = 0.0f;
	if (m_2dCam.y < 0.0f) m_2dCam.y = 0.0f;
	// TODO: такое же сделать для макс

	glm::vec2 realMousePos = app::GetMousePosition();
	glm::ivec2 pos;
	glm::vec2 offset;
	glm::vec2 sizeCell;
	getMouseInfo(realMousePos, pos, offset, sizeCell);
	glm::ivec2 offsetTest = glm::ivec2(0);
	if (offset.x > 0.5f) offsetTest.x = 1;
	if (offset.y > 0.5f) offsetTest.y = 1;

	Cursor.pos = glm::vec2(pos.x + offsetTest.x, pos.y + offsetTest.y);
	Cursor.realPos = glm::vec2(Cursor.pos.x * gridStep, Cursor.pos.y * gridStep);

	if (app::IsMouseButtonPressed(0))
	{
		std::string text = "Mouse pos=" + std::to_string(pos.x) + ":" + std::to_string(pos.y);
		text += "- Offset=" + std::to_string(offset.x) + ":" + std::to_string(offset.y);
		text += "- Real=" + std::to_string(realMousePos.x) + ":" + std::to_string(realMousePos.y);
		LogPrint(text);
		testPoints.push_back(Cursor);
	}
}

void EditorLeftPanel::Draw(float deltaTime)
{
	//if (m_isActive)
	{
		// масштаб экрана равен halfScreenWidth / viewSize, то есть 800 / 100, тогда
		// 10 в вьюве - это 80 реально
		// а 80 в реале (например мышь) - это 10 в вьюве

		const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
		glm::mat4 proj = glm::ortho(0.0f, viewSize, viewSize, 0.0f, -1.0f, 1.0f);

		glm::mat4 view = glm::lookAt(glm::vec3(m_2dCam.x, m_2dCam.y, -0.5f), glm::vec3(m_2dCam.x, m_2dCam.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glDisable(GL_DEPTH_TEST);
		glViewport(0.0f, 0.0f, halfScreenWidth, (float)app::GetWindowHeight());
		glScissor(0.0f, 0.0f, halfScreenWidth, (float)app::GetWindowHeight());
		render::Bind(m_shader);
		render::SetUniform(m_uniformProj, proj);
		render::SetUniform(m_uniformView, view);
		render::SetUniform(m_uniformColor, glm::vec3(0.1f, 0.2f, 0.3f));

		for (int x = 0; x <= gridSize; x += gridStep)
		{
			//if (x <= viewSize + m_2dCam.x) // TODO: проверить что невидимое выброшено (через рендердок)
			{
				glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f));
				world = glm::scale(world, glm::vec3(gridLineSize, 1.0f, 1.0f));
				render::SetUniform(m_uniformWorld, world);
				render::Draw(m_vao);
			}

			//if (x <= viewSize + m_2dCam.y) // TODO: проверить что невидимое выброшено (через рендердок)
			{
				glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, x, 0.0f));
				world = glm::scale(world, glm::vec3(1.0f, gridLineSize, 1.0f));
				render::SetUniform(m_uniformWorld, world);
				render::Draw(m_vao);
			}
		}

		render::SetUniform(m_uniformColor, glm::vec3(1.0f, 1.0f, 1.0f));
		for (int i = 0; i < testPoints.size(); i++)
		{
			glm::mat4 world = glm::translate(glm::mat4(1.0f),
				glm::vec3(
					testPoints[i].realPos.x,
					testPoints[i].realPos.y,
					0.0f));
			world = glm::scale(world, glm::vec3(gridLineSize));
			render::SetUniform(m_uniformWorld, world);
			render::Draw(m_vao);
		}

		// draw cursor
		render::SetUniform(m_uniformColor, glm::vec3(1.0f, 1.0f, 0.3f));
			glm::mat4 world = glm::translate(glm::mat4(1.0f),
				glm::vec3(
					Cursor.realPos.x,
					Cursor.realPos.y,
					0.0f));
			world = glm::scale(world, glm::vec3(gridLineSize*2));
			render::SetUniform(m_uniformWorld, world);
			render::Draw(m_vao);
	}
}

void EditorLeftPanel::SetActive(bool active)
{
	m_isActive = active;
}

void EditorLeftPanel::getMouseInfo(const glm::vec2& realPos, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell)
{
	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
	sizeCell.x = halfScreenWidth / viewSize * gridStep;
	sizeCell.y = (float)app::GetWindowHeight() / viewSize * gridStep;
	
	float posX = realPos.x / sizeCell.x + m_2dCam.x;
	float posY = realPos.y / sizeCell.y + m_2dCam.y;

	posInMap.x = static_cast<int>(posX);
	posInMap.y = static_cast<int>(posY);

	offset.x = posX - (float)posInMap.x;
	offset.y = posY - (float)posInMap.y;
}