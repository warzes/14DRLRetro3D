#include "stdafx.h"
#include "bsp.h"

float planeDistance(const glm::vec3& normal, const float offset, const glm::vec3& point)
{
	return point.x * normal.x + point.y * normal.y + point.z * normal.z + offset;
}

float planeDistance(const glm::vec4& plane, const glm::vec3& point)
{
	return point.x * plane.x + point.y * plane.y + point.z * plane.z + plane.w;
}

no_alias glm::vec3 planeHit(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& plane)
{
	const glm::vec3 dir = v1 - v0;
	const float d = planeDistance(plane, v0);
	const glm::vec3 pos = v0 - (d / glm::dot({ plane.x, plane.y, plane.z }, dir)) * dir; // TODO: проверить (glm::vec3&)plane
	return pos;
}

void BTri::Split(BTri* dest, int& nPos, int& nNeg, const glm::vec4& plane, const float epsilon) const 
{
	float d[3];
	for (int i = 0; i < 3; i++) 
	{
		d[i] = planeDistance(plane, v[i]);
	}

	int first = 2;
	int second = 0;
	while (!(d[second] > epsilon && d[first] <= epsilon)) 
	{
		first = second;
		second++;
	}

	// Positive triangles
	nPos = 0;
	glm::vec3 h = planeHit(v[first], v[second], plane);
	do 
	{
		first = second;
		second++;
		if (second >= 3) second = 0;

		dest->v[0] = h;
		dest->v[1] = v[first];
		if (d[second] > epsilon)
		{
			dest->v[2] = v[second];
		}
		else 
		{
			dest->v[2] = h = planeHit(v[first], v[second], plane);
		}

		dest->data = data;
		dest->Finalize();
		dest++;
		nPos++;
	} while (d[second] > epsilon);

	// Skip zero area triangle
	if (fabsf(d[second]) <= epsilon) 
	{
		first = second;
		second++;
		if (second >= 3) second = 0;
	}

	// Negative triangles
	nNeg = 0;
	do 
	{
		first = second;
		second++;
		if (second >= 3) second = 0;

		dest->v[0] = h;
		dest->v[1] = v[first];
		if (d[second] < -epsilon) {
			dest->v[2] = v[second];
		}
		else {
			dest->v[2] = planeHit(v[first], v[second], plane);
		}

		dest->data = data;
		dest->Finalize();
		dest++;
		nNeg++;
	} while (d[second] < -epsilon);
}

void BTri::Finalize() 
{
	glm::vec3 normal = glm::normalize(glm::cross(v[1] - v[0], v[2] - v[0]));
	float offset = -glm::dot(v[0], normal);

	glm::vec3 edgeNormals[3];
	edgeNormals[0] = cross(normal, v[0] - v[2]);
	edgeNormals[1] = cross(normal, v[1] - v[0]);
	edgeNormals[2] = cross(normal, v[2] - v[1]);

	float edgeOffsets[3];
	edgeOffsets[0] = dot(edgeNormals[0], v[0]);
	edgeOffsets[1] = dot(edgeNormals[1], v[1]);
	edgeOffsets[2] = dot(edgeNormals[2], v[2]);

	plane = glm::vec4(normal, offset);
	edgePlanes[0] = glm::vec4(edgeNormals[0], -edgeOffsets[0]);
	edgePlanes[1] = glm::vec4(edgeNormals[1], -edgeOffsets[1]);
	edgePlanes[2] = glm::vec4(edgeNormals[2], -edgeOffsets[2]);
}

no_alias bool BTri::Intersects(const glm::vec3& v0, const glm::vec3& v1) const
{
	glm::vec3 dir = v0 - v1;
	//	float k = (dot(normal, v0) + offset) / dot(normal, dir);
	float k = planeDistance(plane, v0) / glm::dot({ plane.x, plane.y, plane.z }, dir);

	if (k < 0 || k > 1) return false;

	glm::vec3 pos = v0 - k * dir;

	for (unsigned int i = 0; i < 3; i++) 
	{
		if (planeDistance(edgePlanes[i], pos) < 0)
		{
			//		if (dot(edgeNormals[i], pos) < edgeOffsets[i]){
			return false;
		}
	}
	return true;
}

no_alias bool BTri::IsAbove(const glm::vec3& pos) const 
{
	/*
		return (edgeNormals[0].x * pos.x + edgeNormals[0].y * pos.y + edgeNormals[0].z * pos.z >= edgeOffsets[0] &&
				edgeNormals[1].x * pos.x + edgeNormals[1].y * pos.y + edgeNormals[1].z * pos.z >= edgeOffsets[1] &&
				edgeNormals[2].x * pos.x + edgeNormals[2].y * pos.y + edgeNormals[2].z * pos.z >= edgeOffsets[2]);
	*/
	/*
		return (edgePlanes[0].x * pos.x + edgePlanes[0].y * pos.y + edgePlanes[0].z * pos.z >= -edgePlanes[0].w &&
				edgePlanes[1].x * pos.x + edgePlanes[1].y * pos.y + edgePlanes[1].z * pos.z >= -edgePlanes[1].w &&
				edgePlanes[2].x * pos.x + edgePlanes[2].y * pos.y + edgePlanes[2].z * pos.z >= -edgePlanes[2].w);
	*/
	return (planeDistance(edgePlanes[0], pos) >= 0 && planeDistance(edgePlanes[1], pos) >= 0 && planeDistance(edgePlanes[2], pos) >= 0);
}

no_alias float BTri::GetDistance(const glm::vec3& pos) const
{
	int k = 2;
	for (int i = 0; i < 3; i++) {
		float d = planeDistance(edgePlanes[i], pos);
		if (d < 0) {
			// Project onto the line between the points
			glm::vec3 dir = v[i] - v[k];
			float c = glm::dot(dir, pos - v[k]) / dot(dir, dir);

			glm::vec3 d;
			if (c >= 1) {
				d = v[i];
			}
			else {
				d = v[k];
				if (c > 0) d += c * dir;
			}

			return length(pos - d);
		}

		k = i;
	}

	return fabsf(planeDistance(plane, pos));
}

BNode::~BNode() 
{
	delete back;
	delete front;
}

no_alias bool BNode::Intersects(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& dir, glm::vec3* point, const BTri** triangle) const 
{
#if 0
	float d0 = planeDistance(tri.plane, v0);
	float d1 = planeDistance(tri.plane, v1);

	vec3 pos;
	if (d0 > 0) {
		if (d1 <= 0) {
			pos = v0 - (d0 / dot(tri.plane.xyz(), dir)) * dir;
		}

		if (front != NULL && front->intersects(v0, (d1 <= 0) ? pos : v1, dir, point, triangle)) return true;

		if (d1 <= 0) {
			if (tri.isAbove(pos)) {
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (back != NULL && back->intersects(pos, v1, dir, point, triangle)) return true;
		}
	}
	else {
		if (d1 > 0) {
			pos = v0 - (d0 / dot(tri.plane.xyz(), dir)) * dir;
		}

		if (back != NULL && back->intersects(v0, (d1 > 0) ? pos : v1, dir, point, triangle)) return true;

		if (d1 > 0) {
			if (tri.isAbove(pos)) {
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (front != NULL && front->intersects(pos, v1, dir, point, triangle)) return true;
		}
	}

#else
	float d = planeDistance(tri.plane, v0);

	if (d > 0) 
	{
		if (front != NULL && front->Intersects(v0, v1, dir, point, triangle)) return true;
		if (planeDistance(tri.plane, v1) < 0) {
			glm::vec3 pos = v0 - (d / glm::dot({ tri.plane.x, tri.plane.y, tri.plane.z }, dir)) * dir;
			if (tri.IsAbove(pos)) {
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (back != NULL && back->Intersects(v0, v1, dir, point, triangle)) return true;
		}
	}
	else 
	{
		if (back != NULL && back->Intersects(v0, v1, dir, point, triangle)) return true;
		if (planeDistance(tri.plane, v1) > 0) {
			glm::vec3 pos = v0 - (d / glm::dot({ tri.plane.x, tri.plane.y, tri.plane.z }, dir)) * dir;
			if (tri.IsAbove(pos)) {
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (front != NULL && front->Intersects(v0, v1, dir, point, triangle)) return true;
		}
	}
#endif

	return false;
}

no_alias BTri* BNode::IntersectsCached(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& dir) const 
{
#if 0
	float d0 = planeDistance(tri.plane, v0);
	float d1 = planeDistance(tri.plane, v1);

	vec3 pos;

	if (d0 > 0) {
		if (d1 <= 0) {
			pos = v0 - (d0 / dot((vec3&)tri.plane, dir)) * dir;
		}

		if (front != NULL) {
			BTri* tri;
			if (d1 <= 0) {
				tri = front->intersectsCached(v0, pos, dir);
			}
			else {
				tri = front->intersectsCached(v0, v1, dir);
			}
			if (tri) return tri;
		}

		if (d1 <= 0) {
			if (tri.isAbove(pos)) return (BTri*)&tri;
			if (back != NULL) {
				BTri* tri = back->intersectsCached(pos, v1, dir);
				if (tri) return tri;
			}
		}
	}
	else {
		if (d1 > 0) {
			pos = v0 - (d0 / dot((vec3&)tri.plane, dir)) * dir;
		}
		if (back != NULL) {
			BTri* tri;
			if (d1 > 0) {
				tri = back->intersectsCached(v0, pos, dir);
			}
			else {
				tri = back->intersectsCached(v0, v1, dir);
			}
			if (tri) return tri;
		}
		if (d1 > 0) {
			if (tri.isAbove(pos)) return (BTri*)&tri;
			if (front != NULL) {
				BTri* tri = front->intersectsCached(pos, v1, dir);
				if (tri) return tri;
			}
		}
	}

#else

	float d = planeDistance(tri.plane, v0);

	if (d > 0) {
		if (front != NULL) {
			BTri* tri = front->IntersectsCached(v0, v1, dir);
			if (tri) return tri;
		}
		if (planeDistance(tri.plane, v1) < 0) {
			glm::vec3 pos = v0 - (d / glm::dot({ tri.plane.x, tri.plane.y, tri.plane.z }, dir)) * dir;
			if (tri.IsAbove(pos)) return (BTri*)&tri;
			if (back != NULL) {
				BTri* tri = back->IntersectsCached(v0, v1, dir);
				if (tri) return tri;
			}
		}
	}
	else {
		if (back != NULL) {
			BTri* tri = back->IntersectsCached(v0, v1, dir);
			if (tri) return tri;
		}
		if (planeDistance(tri.plane, v1) > 0) {
			glm::vec3 pos = v0 - (d / glm::dot({ tri.plane.x, tri.plane.y, tri.plane.z }, dir)) * dir;
			if (tri.IsAbove(pos)) return (BTri*)&tri;
			if (front != NULL) {
				BTri* tri = front->IntersectsCached(v0, v1, dir);
				if (tri) return tri;
			}
		}
	}
#endif

	return NULL;
}

no_alias bool BNode::PushSphere(glm::vec3& pos, const float radius) const 
{
	float d = planeDistance(tri.plane, pos);

	bool pushed = false;
	if (fabsf(d) < radius) {
		if (tri.IsAbove(pos)) {
			//			pos += (radius - d) * tri.normal; 
			pos += (radius - d) * glm::vec3(tri.plane);
			pushed = true;
		}
	}

	if (front != NULL && d > -radius) pushed |= front->PushSphere(pos, radius);
	if (back != NULL && d < radius) pushed |= back->PushSphere(pos, radius);

	return pushed;
}

no_alias void BNode::GetDistance(const glm::vec3& pos, float& minDist) const
{
	float d = planeDistance(tri.plane, pos);

	float dist = tri.GetDistance(pos);
	if (dist < minDist) {
		minDist = dist;
	}

	if (back && d < minDist) {
		back->GetDistance(pos, minDist);
	}

	if (front && -d < minDist) {
		front->GetDistance(pos, minDist);
	}
}

void BNode::Read(FILE* file)
{
	fread(&tri.v, sizeof(tri.v), 1, file);
	tri.Finalize();

	int flags = 0;
	fread(&flags, sizeof(int), 1, file);
	if (flags & 1) {
		back = new BNode;
		back->Read(file);
	}
	else back = NULL;
	if (flags & 2) {
		front = new BNode;
		front->Read(file);
	}
	else front = NULL;
}

void BNode::Write(FILE* file) const
{
	fwrite(&tri.v, sizeof(tri.v), 1, file);
	int flags = 0;
	if (back) flags |= 1;
	if (front) flags |= 2;
	fwrite(&flags, sizeof(int), 1, file);
	if (back) back->Write(file);
	if (front) front->Write(file);
}
/*
void BNode::build(Array <BTri> &tris){
	uint index = 0;
	int minScore = 0x7FFFFFFF;
	for (uint i = 0; i < tris.getCount(); i++){
		int score = 0;
		int diff = 0;
		for (uint k = 0; k < tris.getCount(); k++){
			uint neg = 0, pos = 0;
			for (uint j = 0; j < 3; j++){
//				float dist = dot(tris[k].v[j], tris[i].normal) + tris[i].offset;
				float dist = planeDistance(tris[i].plane, tris[k].v[j]);
				if (dist < 0) neg++; else pos++;
			}
			if (pos){
				if (neg) score += 3; else diff++;
			} else diff--;
		}
		score += abs(diff);
		if (score < minScore){
			minScore = score;
			index = i;
		}
	}
	tri = tris[index];
	tris.fastRemove(index);

	Array <BTri> backTris;
	Array <BTri> frontTris;
	for (uint i = 0; i < tris.getCount(); i++){
		uint neg = 0, pos = 0;
		for (uint j = 0; j < 3; j++){
//			float dist = dot(tris[i].v[j], tri.normal) + tri.offset;
			float dist = planeDistance(tri.plane, tris[i].v[j]);
			if (dist < 0) neg++; else pos++;
		}
		if (neg) backTris.add(tris[i]);
		if (pos) frontTris.add(tris[i]);
	}
	//tris.clear();

	if (backTris.getCount() > 0){
		back = new BNode;
		back->build(backTris);
	} else back = NULL;

	if (frontTris.getCount() > 0){
		front = new BNode;
		front->build(frontTris);
	} else front = NULL;
}
*/

void BNode::Build(Array<BTri>& tris, const int splitCost, const int balCost, const float epsilon)
{
	uint32_t index = 0;
	int minScore = 0x7FFFFFFF;

	for (uint32_t i = 0; i < tris.getCount(); i++) 
	{
		int score = 0;
		int diff = 0;
		for (uint32_t k = 0; k < tris.getCount(); k++)
		{
			uint32_t neg = 0, pos = 0;
			for (uint32_t j = 0; j < 3; j++) 
			{
				float dist = planeDistance(tris[i].plane, tris[k].v[j]);
				if (dist < -epsilon) neg++; else
					if (dist > epsilon) pos++;
			}
			if (pos) 
			{
				if (neg) score += splitCost; else diff++;
			}
			else
			{
				if (neg) diff--; else diff++;
			}
		}
		score += balCost * abs(diff);
		if (score < minScore) {
			minScore = score;
			index = i;
		}
	}

	tri = tris[index];
	tris.fastRemove(index);

	Array <BTri> backTris;
	Array <BTri> frontTris;
	for (uint32_t i = 0; i < tris.getCount(); i++) 
	{
		uint32_t neg = 0, pos = 0;
		for ( uint32_t j = 0; j < 3; j++) 
		{
			float dist = planeDistance(tri.plane, tris[i].v[j]);
			if (dist < -epsilon) neg++; else
				if (dist > epsilon) pos++;
		}

		if (neg) 
		{
			if (pos) 
			{
				BTri newTris[3];
				int nPos, nNeg;
				tris[i].Split(newTris, nPos, nNeg, tri.plane, epsilon);
				for (int i = 0; i < nPos; i++) 
				{
					frontTris.add(newTris[i]);
				}
				for (int i = 0; i < nNeg; i++) 
				{
					backTris.add(newTris[nPos + i]);
				}
			}
			else {
				backTris.add(tris[i]);
			}
		}
		else {
			frontTris.add(tris[i]);
		}
	}
	tris.reset();

	if (backTris.getCount() > 0) {
		back = new BNode;
		back->Build(backTris, splitCost, balCost, epsilon);
	}
	else back = NULL;

	if (frontTris.getCount() > 0) {
		front = new BNode;
		front->Build(frontTris, splitCost, balCost, epsilon);
	}
	else front = NULL;
}

void BSP::AddTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, void* data) 
{
	BTri tri;

	tri.v[0] = v0;
	tri.v[1] = v1;
	tri.v[2] = v2;
	tri.data = data;

	tri.Finalize();

	m_tris.add(tri);
}

void BSP::Build(const int splitCost, const int balCost, const float epsilon) 
{
	//	int nTris = tris.getCount();
	m_top = new BNode;
	m_top->Build(m_tris, splitCost, balCost, epsilon);
}

no_alias bool BSP::Intersects(const glm::vec3& v0, const glm::vec3& v1, glm::vec3* point, const BTri** triangle) const
{
	if (m_top != NULL) return m_top->Intersects(v0, v1, v1 - v0, point, triangle);
	return false;
}

bool BSP::IntersectsCached(const glm::vec3& v0, const glm::vec3& v1) 
{
	if (m_top != NULL) {
		if (m_cache) {
			if ( m_cache->Intersects(v0, v1)) return true;
		}
		m_cache = m_top->IntersectsCached(v0, v1, v1 - v0);
		return (m_cache != NULL);
	}

	return false;
}

bool BSP::PushSphere(glm::vec3& pos, const float radius) const
{
	if (m_top != NULL) return m_top->PushSphere(pos, radius);

	return false;
}

no_alias float BSP::GetDistance(const glm::vec3& pos) const 
{
	float dist = FLT_MAX;

	if ( m_top != NULL) m_top->GetDistance(pos, dist);

	return dist;
}


no_alias bool BSP::IsInOpenSpace(const glm::vec3& pos) const
{
	if ( m_top != NULL) 
	{
		BNode* node = m_top;
		while (true) 
		{
			float d = planeDistance(node->tri.plane, pos);

			if (d > 0) 
			{
				if (node->front)
				{
					node = node->front;
				}
				else return true;
			}
			else {
				if (node->back) 
				{
					node = node->back;
				}
				else return false;
			}
		}
	}

	return false;
}

bool BSP::LoadFile(const char* fileName) 
{
	FILE* file = fopen(fileName, "rb");
	if (file == NULL) return false;

	delete m_top;

	m_top = new BNode;
	m_top->Read(file);
	fclose(file);

	return true;
}

bool BSP::SaveFile(const char* fileName) const 
{
	if ( m_top == NULL) return false;

	FILE* file = fopen(fileName, "wb");
	if (file == NULL) return false;

	m_top->Write(file);
	fclose(file);

	return true;
}