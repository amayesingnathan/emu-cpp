#pragma once

#include "VRAM.h"
#include "OAM.h"
#include "Screen.h"

namespace GB {

	class PPU
	{
	public:

	private:
		VideoRAM mVRAM;
		OAM mOAM;
		Screen mDisplay;
	};
}