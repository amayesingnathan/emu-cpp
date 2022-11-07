#pragma once

#include "VRAM.h"
#include "OAM.h"
#include "Screen.h"

namespace GB {

	class PPU
	{
	public:
		PPU() = default;

	public:
		void update(int cycles) {}

	private:
		VideoRAM mVRAM;
		SpriteMemory mOAM;
		Screen mDisplay;
	};
}