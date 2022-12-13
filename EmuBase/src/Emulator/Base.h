#pragma once

#include "Core/Timing.h"
#include "Graphics/Texture.h"
#include "IO/Event.h"
#include "ImGui/ImGuiHandler.h"

namespace Emu {

	class Base
	{
	public:
		virtual ~Base() {}

		virtual void update() = 0;
		virtual void imguiRender() = 0;
		virtual void onEvent(Event& e) = 0;
		virtual uint getDisplayTex() = 0;
		virtual Duration getFrameTime() = 0;
	};
}