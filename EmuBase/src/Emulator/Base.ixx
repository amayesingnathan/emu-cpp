export module EmuBase.Emulator;

import EmuBase.Core.Common;
import EmuBase.Core.Timing;
import EmuBase.Graphics.Texture;
import EmuBase.IO.Event;
import EmuBase.ImGui.Handler;

namespace Emu {

	export class Base
	{
	public:
		virtual ~Base() {}

		virtual void update() = 0;
		virtual void uiRender() = 0;
		virtual void onEvent(Event& e) = 0;
		virtual uint getDisplayTex() = 0;
		virtual Duration getFrameTime() = 0;
	};
}