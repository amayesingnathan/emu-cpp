#pragma once

#include "Screen.h"

namespace GB {

	class PPU
	{
	public:
		PPU() = default;

	public:
		void update(int cycles) {}

	private:
		Screen mDisplay;
	};
}