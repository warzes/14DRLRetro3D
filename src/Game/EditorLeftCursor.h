#pragma once

class EditorLeftViewport;
class EditorLeftDrawHelper;

class EditorLeftCursor
{
public:
	bool Create();
	void Destroy();

	void Update(const EditorLeftViewport& viewport);
	void Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer);

	const glm::vec2& PosToMap() const { return m_posToMap; }

private:
	void getCursorToMap(const EditorLeftViewport& viewport);

	glm::vec2 m_posToMap;
	glm::vec2 m_posToScreen;
};