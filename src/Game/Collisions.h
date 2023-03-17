#pragma once

//=============================================================================
// Collisions
//=============================================================================
inline bool Intersection2D(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, glm::vec2& outPoint)
{
	// Store the values for fast access and easy equations-to-code conversion
	const float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	const float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
	const float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if( d == 0 ) return false;

	// Get the x and y
	const float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
	const float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
	const float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

	// Check if the x and y coordinates are within both lines
	if( x < std::min(x1, x2) || x > std::max(x1, x2) ||
		x < std::min(x3, x4) || x > std::max(x3, x4) ) return false;
	if( y < std::min(y1, y2) || y > std::max(y1, y2) ||
		y < std::min(y3, y4) || y > std::max(y3, y4) ) return false;

	// Return the point of intersection
	outPoint.x = x;
	outPoint.y = y;
	return true;
}