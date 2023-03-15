#pragma once

struct SectorEditorVertex
{
	glm::vec2 pos;
};

struct SectorEditorWall
{
	SectorEditorVertex p1;
	SectorEditorVertex p2;
};


// идея в том что изменения производить если были изменения в редакторе
extern bool EditorDataChange;

// редактор работает так:
//	раставляются вершины путем клика мышкой
//	когда будет щелчок по первой секте, тогда создастся сектор
extern SectorEditorWall TempEditorCurrentWall;
extern std::list<SectorEditorWall> TempEditorWalls;
extern unsigned TempEditorVertexNum;