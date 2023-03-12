#pragma once

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

	void add_compare_func(const char* name, comparator_t compare_func);
	comparator_t get_compare_func(const char* name);

#ifdef __cplusplus
}
#endif