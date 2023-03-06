#if defined(_MSC_VER)
#	pragma warning(push, 0)
#	pragma warning(disable : 4255)
#	pragma warning(disable : 4706)
#	pragma warning(disable : 4820)
#	pragma warning(disable : 5045)
#	define _CRT_SECURE_NO_WARNINGS
#endif

// Tool update in https://github.com/warzes/glfw-single-header

#define _GLFW_IMPLEMENTATION
#include <glfw.h>
#undef _GLFW_IMPLEMENTATION

#if defined(_MSC_VER)
#	undef _CRT_SECURE_NO_WARNINGS
#	pragma warning(pop)
#endif