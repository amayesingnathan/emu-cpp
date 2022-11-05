#pragma once

#include "VRAM.h"
#include "OAM.h"
#include "Screen.h"

namespace GB {

	class PPU
	{
	public:

	private:
		VRAM mVRAM;
		OAM mOAM;
		Screen mDisplay;
	};
}