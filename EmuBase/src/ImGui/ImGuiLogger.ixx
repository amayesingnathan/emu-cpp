module;

#include "imgui.h"

export module EmuBase.ImGui.Logger;

export namespace Emu {

	class LogWidget
	{
	public:
		LogWidget();

		void clear();
		void draw();

	private:
		ImGuiTextBuffer mBuffer;
		ImGuiTextFilter mFilter;
		ImVector<int> mLineOffsets;
		bool mAutoScroll; 

		friend class ImGuiSink;
	};
}