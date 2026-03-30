/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#include "console.h"

#include "Core/logger.h"

Console::Console() : EditorPanel("Logger")
{
    m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
}

void Console::begin_frame()
{
    EditorPanel::begin_frame();
}

void Console::update()
{
    EditorPanel::update();

    ImGui::Button("Clear", ImVec2(50, 20));
    ImGui::SameLine();
    static char search_buffer[64] = "";
    ImGui::InputTextWithHint("##log_filter", "Filter logs by search...", search_buffer, IM_ARRAYSIZE(search_buffer));

    ImGui::Spacing();

    ImGui::BeginTable("##console_text", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY);
    {
        for (const auto& log_entry : Logger::get_log_entries())
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 25.0f);
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", log_entry.c_str());
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndTable();
}

void Console::end_frame()
{
    EditorPanel::end_frame();
}
