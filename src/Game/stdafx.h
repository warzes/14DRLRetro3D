#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 0)
//#	pragma warning(disable : 5262)
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

#include <glad/gl.h>

#include <glfw.h>

/*
Left handed
	Y   Z
	|  /
	| /
	|/___X
*/
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/hash.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtx/matrix_decompose.hpp>
//#include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/normal.hpp>

#include <stb_image.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif