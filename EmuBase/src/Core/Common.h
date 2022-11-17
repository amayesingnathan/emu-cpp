#pragma once

#include "glpch.h"

#define GL_ASSERT(x, ...) assert(x)

namespace GL {

	using uint = unsigned int;

	using usize = size_t;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}