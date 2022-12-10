#pragma once

#include "glpch.h"

#define GL_ASSERT(x, ...) assert(x)
#define GL_SASSERT(...) static_assert(__VA_ARGS__)

namespace Emu {

	using uint = unsigned int;

	using usize = size_t;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}