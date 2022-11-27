#pragma once

#include "spdlog/sinks/base_sink.h"
#include "imgui.h"

namespace Emu {

	class LogWidget;

	class ImGuiSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		ImGuiSink(LogWidget& widget)
			: mWidget(&widget) {}

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;

		void flush_() override {}

	private:
		LogWidget* mWidget = nullptr;
	};

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