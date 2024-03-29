#include "stdafx.h"
#include "EditorLeftViewport.h"
#include "EditorConstant.h"
#include "Engine.h"
//-----------------------------------------------------------------------------
void EditorLeftViewport::Update(float deltaTime)
{
	updatePositionCamera(deltaTime);

	m_halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
	m_screenViewPort = glm::vec4(0.f, 0.f, m_halfScreenWidth, app::GetWindowHeight());

	// TODO: нужно ли так делать?
	float aspectX = 1.0f;
	float aspectY = 1.0f;
	// TODO: не работает - дает лишний сдвиг по высоте
	//if( (int)m_halfScreenWidth > app::GetWindowHeight() ) aspectX = m_halfScreenWidth / (float)app::GetWindowHeight();
	//else aspectY = (float)app::GetWindowHeight() / m_halfScreenWidth;

	m_orthoProjection = glm::ortho(
		0.0f, (float)EditorViewSize * aspectX,
		(float)EditorViewSize * aspectY, 0.0f,
		-1.0f, 1.0f);

	constexpr glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	m_view = glm::lookAt({ m_camPos.x, m_camPos.y, -0.5f }, { m_camPos.x, m_camPos.y, 0.0f }, up);
}
//-----------------------------------------------------------------------------
void EditorLeftViewport::SetOpenGLViewport()
{
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, (GLsizei)m_screenViewPort.z, (GLsizei)m_screenViewPort.w);
	glScissor( 0, 0, (GLsizei)m_screenViewPort.z, (GLsizei)m_screenViewPort.w);
}
//-----------------------------------------------------------------------------
glm::vec3 EditorLeftViewport::PosToScreenSpace(const glm::vec3& worldPos, bool useCameraView) const
{
	return glm::project(worldPos, useCameraView ? m_view : glm::mat4(1.0f), m_orthoProjection, m_screenViewPort);
}
//-----------------------------------------------------------------------------
glm::vec3 EditorLeftViewport::PosToWorldSpace(const glm::vec3& screenPos, bool useCameraView) const
{
	return glm::unProject(screenPos, useCameraView ? m_view : glm::mat4(1.0f), m_orthoProjection, m_screenViewPort);
}
//-----------------------------------------------------------------------------
glm::ivec2 EditorLeftViewport::GetPosInMap(const glm::vec3& screenPos) const
{
	const glm::vec3 posToMap = PosToWorldSpace(screenPos, true);

	float tX = posToMap.x / EditorGridStep;
	float tY = posToMap.y / EditorGridStep;
	if( tX < 0.0f ) tX = 0.0f;
	if( tX < 0.0f ) tX = 0.0f;
	if( tX >= EditorMapGridSize ) tX = EditorMapGridSize - 1;
	if( tY >= EditorMapGridSize ) tY = EditorMapGridSize - 1;

	const int tiX = (int)(tX);
	const int tiY = (int)(tY);

	return {tiX, tiY};
}
//-----------------------------------------------------------------------------
void EditorLeftViewport::updatePositionCamera(float deltaTime)
{
	if( app::IsKeyDown(app::KEY_A) ) m_camPos.x -= CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_D) ) m_camPos.x += CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_W) ) m_camPos.y -= CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_S) ) m_camPos.y += CameraSpeedInEditorLeftPanel * deltaTime;
	if( m_camPos.x < 0.0f ) m_camPos.x = 0.0f;
	if( m_camPos.y < 0.0f ) m_camPos.y = 0.0f;
	if( m_camPos.x > EditorMapGridSize - EditorViewSize ) m_camPos.x = EditorMapGridSize - EditorViewSize;
	if( m_camPos.y > EditorMapGridSize - EditorViewSize ) m_camPos.y = EditorMapGridSize - EditorViewSize;
	//std::string text = "cam=" + std::to_string(cam.x) + "-" + std::to_string(cam.y);
	//LogPrint(text);
}
//-----------------------------------------------------------------------------