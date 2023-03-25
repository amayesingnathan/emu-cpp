module;

#include "spdlog/sinks/base_sink.h"

export module EmuBase.ImGui.Sink;

import EmuBase.ImGui.Logger;

export namespace Emu {

	class ImGuiSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		ImGuiSink(LogWidget& widget)
			: mWidget(&widget) {}

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			if (!mWidget)
				return;

			int old_size = mWidget->mBuffer.size();
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
			mWidget->mBuffer.append(fmt::to_string(formatted).c_str());
			for (int new_size = mWidget->mBuffer.size(); old_size < new_size; old_size++)
			{
				if (mWidget->mBuffer[old_size] == '\n')
					mWidget->mLineOffsets.push_back(old_size + 1);
			}
		}

		void flush_() override {}

	private:
		LogWidget* mWidget = nullptr;
	};
}