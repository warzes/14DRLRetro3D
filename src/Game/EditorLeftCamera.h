#pragma once

// камера для левой (2д) стороны редактора. Начало координат камеры равно левой вехней точке карты

class EditorLeftCamera
{
public:
	void Update(float deltaTime);

	glm::vec2 cam = glm::vec2(0.0f, 0.0f);
};