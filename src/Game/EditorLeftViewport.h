#pragma once

class EditorLeftViewport
{
public:
	void Update(float deltaTime);
	void SetOpenGLViewport();

	const glm::mat4& GetOrthoProjection() const { return m_orthoProjection; }
	const glm::mat4& GetView() const { return m_view; }
	const glm::vec4& GetScreen() const { return m_screenViewPort; }
	//float GetHalfScreenWidth() const { return m_halfScreenWidth; }
	float GetWidth() const { return m_screenViewPort.z; }
	float GetHeight() const { return m_screenViewPort.w; }
	const glm::vec2& GetCameraPosition() const { return m_camPos; }

	glm::vec3 PosToScreenSpace(const glm::vec3& worldPos, bool useCameraView = true) const;
	glm::vec3 PosToWorldSpace(const glm::vec3& screenPos, bool useCameraView = true) const;

	// ��������� ���������� �� ����������� � ���������� ������ ����� ���� [gridSize][gridSize]
	glm::ivec2 GetPosInMap(const glm::vec3& screenPos) const;

	// outWorldPos - ������� ��������� � ������� ������������
	// outPosInMap - ������� �� �����, ��� ����� ������������� [gridSize][gridSize]. ����� �����
	// outSizeCell - ������ ������
	// outOffset - ���������� �� ������-�������� ���� ������ �� ���������
	// useCameraView - ������� ������� ����������� ������ (����� ������ � �������� ����)
	void GetCursorInfo(glm::vec3& outWorldPos, glm::ivec2& outPosInMap, glm::vec2& outSizeCell, glm::vec2& outOffset, bool useCameraView = true) const;

private:
	void updatePositionCamera(float deltaTime);
	glm::mat4 m_orthoProjection;
	glm::mat4 m_view;
	glm::vec4 m_screenViewPort;
	float m_halfScreenWidth = 0;

	glm::vec2 m_camPos = glm::vec2(0.0f, 0.0f);
};