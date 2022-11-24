#pragma once

#include "Graphics/Texture.h"
#include "IO/Input.h"

namespace Emu {

	class Base
	{
	public:
		virtual ~Base() {}

		virtual void update() = 0;
		virtual ActionCallback getActionCallback() = 0;
		virtual uint getDisplayTex() = 0;
		virtual Duration getFrameTime() = 0;
	};
}