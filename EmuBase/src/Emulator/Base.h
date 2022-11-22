#pragma once

#include "IO/Input.h"

namespace Emu {

	struct Specification
	{
		uint displayWidth, displayHeight;
		std::string_view gamePath;
	};

	class Base
	{
	public:
		virtual ~Base() {}

		virtual void update() = 0;
		virtual ActionCallback getActionCallback() = 0;
		virtual uint getFBO() = 0;
	};
}