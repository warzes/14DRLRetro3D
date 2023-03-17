#pragma once

class EditorLeftViewport;
class EditorLeftDrawHelper;

class EditorLeftMap
{
public:
	bool Create();
	void Destroy();

	void Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const;

	void GetCursorToMap(const EditorLeftViewport& viewport, glm::vec2& outPosToMap, glm::vec2& outPosToScreen) const;

private:
	void drawSectors(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const;
};