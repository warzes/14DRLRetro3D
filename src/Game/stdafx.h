#pragma once

#define _CRT_SECURE_NO_WARNINGS // TODO: delete

#if defined(_MSC_VER)
#	pragma warning(disable : 4514)
#	pragma warning(disable : 4820)
#	pragma warning(disable : 5045)
#	pragma warning(push, 0)
#	pragma warning(disable : 4365)
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream> // TODO: удалить

#if defined(_WIN32)
#	include <glad/gl.h>
#endif // _WIN32

#if defined(__ANDROID__)
#endif // __ANDROID__

#if defined(__linux__)
#endif // __linux__

#if defined(__EMSCRIPTEN__)
#	define GLFW_INCLUDE_ES3
//#	define GL_GLEXT_PROTOTYPES
//#	define EGL_EGLEXT_PROTOTYPES
#	include <emscripten/emscripten.h>
#	include <emscripten/html5.h>
#endif // __EMSCRIPTEN__

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
#include <glm/gtx/hash.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtx/matrix_decompose.hpp>
//#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/normal.hpp>

#include <stb_image.h>

#include <polypartition/polypartition.h>

#include <tiny_obj_loader.h>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif