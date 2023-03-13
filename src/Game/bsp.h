#pragma once

#include "ArrayH.h"

#if defined(_MSC_VER)
#	define assume(x) __assume(x)
#	define no_alias __declspec(noalias)
#else
#	define assume(x)
#	define no_alias
#endif

struct BTri 
{
	void Split(BTri* dest, int& nPos, int& nNeg, const glm::vec4& plane, const float epsilon) const;
	void Finalize();

	bool Intersects(const glm::vec3& v0, const glm::vec3& v1) const;

	bool IsAbove(const glm::vec3& pos) const;

	float GetDistance(const glm::vec3& pos) const;

	glm::vec4 plane;
	glm::vec4 edgePlanes[3];

	glm::vec3 v[3];
	/*
		vec3 edgeNormals[3];
		float edgeOffsets[3];
		vec3 normal;
		float offset;
	*/
	void* data;
};

struct BNode 
{
	~BNode();

	bool Intersects(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& dir, glm::vec3* point, const BTri** triangle) const;
	BTri* IntersectsCached(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& dir) const;

	bool PushSphere(glm::vec3& pos, const float radius) const;
	void GetDistance(const glm::vec3& pos, float& minDist) const;

	void Build(Array<BTri>& tris, const int splitCost, const int balCost, const float epsilon);

	void Read(FILE* file);
	void Write(FILE* file) const;

	BNode* back;
	BNode* front;

	BTri tri;
};

class BSP 
{
public:
	~BSP() { delete m_top; }

	void AddTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, void* data = nullptr);
	void Build(const int splitCost = 3, const int balCost = 1, const float epsilon = 0.001f);

	bool Intersects(const glm::vec3& v0, const glm::vec3& v1, glm::vec3* point = nullptr, const BTri** triangle = nullptr) const;
	bool IntersectsCached(const glm::vec3& v0, const glm::vec3& v1);

	bool PushSphere(glm::vec3& pos, const float radius) const;
	float GetDistance(const glm::vec3& pos) const;

	bool IsInOpenSpace(const glm::vec3& pos) const;

	bool LoadFile(const char* fileName);
	bool SaveFile(const char* fileName) const;

private:
	Array<BTri> m_tris;
	BNode* m_top = nullptr;
	BTri* m_cache = nullptr;
};