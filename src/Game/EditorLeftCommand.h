#pragma once

class EditorLeftCursor;
class EditorLeftMap;

class EditorLeftCommand
{
public:
	void Update(const EditorLeftCursor& cursor, EditorLeftMap& map);

private:
	bool checkCursorPoint() const;
	bool addPoint();
	void buildEditorSector();

	bool selectPoint();
	bool movePoint();
};