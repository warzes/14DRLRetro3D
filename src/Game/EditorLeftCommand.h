#pragma once

class EditorLeftCursor;
class EditorLeftMap;
struct SectorEditorSector;

class EditorLeftCommand
{
public:
	void Update(const EditorLeftCursor& cursor, EditorLeftMap& map);

private:
	void buildEditorSector();
	void buildGeomSector(SectorEditorSector& sector);
};