#pragma once

namespace Emu {

	class Base
	{
	public:
		virtual ~Base() {}

		virtual void update() = 0;
		virtual uint getFBO() = 0;
	};
}