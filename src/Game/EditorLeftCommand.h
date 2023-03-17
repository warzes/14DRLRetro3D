#pragma once

class EditorLeftCursor;
class EditorLeftMap;
struct SectorEditorSector;

class EditorLeftCommand
{
public:
	void Update(const EditorLeftCursor& cursor, EditorLeftMap& map);

private:
	bool checkCursorPoint() const;
	bool addPoint();
	void buildEditorSector();
};