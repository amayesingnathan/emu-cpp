#include "glpch.h"
#include "ImGuiLogger.h"

namespace Emu {

	void ImGuiSink::sink_it_(const spdlog::details::log_msg& msg)
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

	LogWidget::LogWidget()
		: mAutoScroll(true)
	{
		clear();
	}

	void LogWidget::clear()
	{
		mBuffer.clear();
		mLineOffsets.clear();
		mLineOffsets.push_back(0);
	}

	void LogWidget::draw()
	{
        ImGui::Begin("Log");

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &mAutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clearLog = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copyLog = ImGui::Button("Copy");
        ImGui::SameLine();
        mFilter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clearLog)
            clear();
        if (copyLog)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = mBuffer.begin();
        const char* buf_end = mBuffer.end();
        if (mFilter.IsActive())
        {
            for (int line_no = 0; line_no < mLineOffsets.Size; line_no++)
            {
                const char* line_start = buf + mLineOffsets[line_no];
                const char* line_end = (line_no + 1 < mLineOffsets.Size) ? (buf + mLineOffsets[line_no + 1] - 1) : buf_end;
                if (mFilter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            ImGuiListClipper clipper;
            clipper.Begin(mLineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + mLineOffsets[line_no];
                    const char* line_end = (line_no + 1 < mLineOffsets.Size) ? (buf + mLineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (mAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
	}
}