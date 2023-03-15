#pragma once

class EditorLeftCursor;
class EditorLeftMap;

class EditorLeftCommand
{
public:
	void Update(const EditorLeftCursor& cursor, EditorLeftMap& map);
};