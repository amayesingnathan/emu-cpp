module;

#include "imgui.h"

module EmuBase.ImGui.Logger;

namespace Emu {

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