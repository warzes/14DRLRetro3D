#pragma once

// ������ ��� ����� (2�) ������� ���������. ������ ��������� ������ ����� ����� ������ ����� �����

class EditorLeftCamera
{
public:
	void Update(float deltaTime);

	glm::vec2 cam = glm::vec2(0.0f, 0.0f);
};