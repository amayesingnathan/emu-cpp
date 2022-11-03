#pragma once

#include "gbpch.h"

#define GB_ASSERT(x, ...) assert(x)
#define GB_SASSERT(x, ...) static_assert(x)

#define if_c if constexpr
#define GB_CONST inline static constexpr