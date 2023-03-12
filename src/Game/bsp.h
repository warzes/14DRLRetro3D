#pragma once

#if _MSC_VER >= 1400
// To make MSVC 2005 happy
#pragma warning (disable: 4996)
#  define assume(x) __assume(x)
#  define no_alias __declspec(noalias)
#else
#  define assume(x)
#  define no_alias
#endif

struct BTri 
{
	void Split(BTri* dest, int& nPos, int& nNeg, const glm::vec4& plane, const float epsilon) const;
	void Finalize();

	bool Intersects(const glm::vec3& v0, const glm::vec3& v1) const;

	bool IsAbove(const glm::vec3& pos) const;
#ifdef USE_SIMD
	bool IsAbove3DNow(v2sf v0XY, v2sf v0Z1) const;
#endif
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
#ifdef USE_SIMD
	bool Intersects3DNow(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& dir) const;
#endif

	bool PushSphere(glm::vec3& pos, const float radius) const;
	void GetDistance(const glm::vec3& pos, float& minDist) const;

	void Build(std::vector<BTri>& tris, const int splitCost, const int balCost, const float epsilon);
	//void Build(Array <BTri> &tris);

	void Read(FILE* file);
	void Write(FILE* file) const;


	BNode* back;
	BNode* front;

	BTri tri;
};

#ifdef USE_SIMD
struct SSETri {
	v4sf plane;
	v4sf edgePlanes[3];
};

align(16) struct SSENode {
	SSETri tri;

	SSENode* back;
	SSENode* front;

	void Build(const BNode* node, SSENode*& sseDest);
};
#endif

class BSP 
{
public:
	BSP() 
	{
		m_top = NULL;
#ifdef USE_SIMD
		m_sseTop = NULL;
		m_sseDest = NULL;
#endif
		m_cache = NULL;
	}
	~BSP() 
	{
#ifdef USE_SIMD
		delete m_sseDest;
#endif
		delete m_top;
	}

	void AddTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, void* data = NULL);
	void Build(const int splitCost = 3, const int balCost = 1, const float epsilon = 0.001f);

	bool Intersects(const glm::vec3& v0, const glm::vec3& v1, glm::vec3* point = NULL, const BTri** triangle = NULL) const;
	bool IntersectsCached(const glm::vec3& v0, const glm::vec3& v1);
#ifdef USE_SIMD
	bool Intersects3DNow(const glm::vec3& v0, const glm::vec3& v1) const;
#endif

	bool PushSphere(glm::vec3& pos, const float radius) const;
	float GetDistance(const glm::vec3& pos) const;

	bool IsInOpenSpace(const glm::vec3& pos) const;
#ifdef USE_SIMD
	bool IsInOpenSpace3DNow(const glm::vec3& pos) const;
	bool IsInOpenSpaceSSE(const glm::vec3& pos) const;
#endif

	bool LoadFile(const char* fileName);
	bool SaveFile(const char* fileName) const;

private:
	std::vector<BTri> m_tris;
	BNode* m_top;
	BTri* m_cache;
#ifdef USE_SIMD
	SSENode* m_sseTop;
	SSENode* m_sseDest;
#endif
};