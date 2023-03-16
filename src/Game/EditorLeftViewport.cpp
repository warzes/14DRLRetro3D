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
		0.0f, (float)viewSize * aspectX,
		(float)viewSize * aspectY, 0.0f,
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

	float tX = posToMap.x / gridStep;
	float tY = posToMap.y / gridStep;
	if( tX < 0.0f ) tX = 0.0f;
	if( tX < 0.0f ) tX = 0.0f;
	if( tX >= gridSize ) tX = gridSize - 1;
	if( tY >= gridSize ) tY = gridSize - 1;

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
	if( m_camPos.x > gridSize - viewSize ) m_camPos.x = gridSize - viewSize;
	if( m_camPos.y > gridSize - viewSize ) m_camPos.y = gridSize - viewSize;
	//std::string text = "cam=" + std::to_string(cam.x) + "-" + std::to_string(cam.y);
	//LogPrint(text);
}
//-----------------------------------------------------------------------------
void EditorLeftViewport::GetCursorInfo(glm::vec3& outWorldPos, glm::ivec2& outPosInMap, glm::vec2& outSizeCell, glm::vec2& outOffset, bool useCameraView) const
{
	const glm::vec2 realMousePos = app::GetMousePosition();
	outWorldPos = PosToWorldSpace({ realMousePos.x, realMousePos.y, 0.0f }, useCameraView);

	const float sizeScaleX = m_screenViewPort.z / viewSize;
	const float sizeScaleY = m_screenViewPort.w / viewSize;

	outSizeCell.x = sizeScaleX * gridStep;
	outSizeCell.y = sizeScaleY * gridStep;

	// функция нерабочая - а возможно уже ненужная

	const float posX = outWorldPos.x + m_camPos.x / gridStep;
	const float posY = outWorldPos.y + m_camPos.y / gridStep;

	const float posX2 = realMousePos.x / outSizeCell.x + m_camPos.x / gridStep;
	const float posY2 = realMousePos.y / outSizeCell.y + m_camPos.y / gridStep;

	outPosInMap.x = static_cast<int>(posX);
	outPosInMap.y = static_cast<int>(posY);

	outOffset.x = posX - (float)outPosInMap.x;
	outOffset.y = posY - (float)outPosInMap.y;
}
//-----------------------------------------------------------------------------