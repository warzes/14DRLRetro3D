#pragma once

inline float sign(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

inline bool PointInTriangle(const glm::vec2& pt, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3)
{
	const float d1 = sign(pt, v1, v2);
	const float d2 = sign(pt, v2, v3);
	const float d3 = sign(pt, v3, v1);

	const bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	const bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}