export module EmuBase.Core.Log;

#pragma warning (push)
#pragma warning(disable: 4005)
export import EmuBase.Core.Common;
export import EmuBase.ImGui.Logger;
#pragma warning(pop)

namespace Emu {

	export namespace Log {

		void Init(LogWidget& widget);

		void Trace(std::string_view msg);
		void Info(std::string_view msg);
		void Warn(std::string_view msg);
		void Error(std::string_view msg);
	}
}