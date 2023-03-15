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


// ���� � ��� ��� ��������� ����������� ���� ���� ��������� � ���������
extern bool EditorDataChange;

// �������� �������� ���:
//	������������ ������� ����� ����� ������
//	����� ����� ������ �� ������ �����, ����� ��������� ������
extern SectorEditorWall TempEditorCurrentWall;
extern std::list<SectorEditorWall> TempEditorWalls;
extern unsigned TempEditorVertexNum;