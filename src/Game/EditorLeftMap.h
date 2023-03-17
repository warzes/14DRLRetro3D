#pragma once

class EditorLeftViewport;
class EditorLeftDrawHelper;

class EditorLeftMap
{
public:
	bool Create();
	void Destroy();

	void Draw(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const;

private:
	void drawSectors(const EditorLeftViewport& viewport, const EditorLeftDrawHelper& drawer) const;
};