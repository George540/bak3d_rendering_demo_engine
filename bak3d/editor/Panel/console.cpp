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

#include "imgui_b3d_extensions.h"
#include "Core/logger.h"

using namespace std;

namespace
{
    float calculate_remaining_width(const string& text)
    {
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float text_width = ImGui::CalcTextSize(text.c_str()).x;
        const float avail_width = ImGui::GetContentRegionAvail().x;
        const float input_width = avail_width - text_width - spacing;
        return input_width;
    }
}

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

    // Clear button
    if (ImGui::Button("Clear", ImVec2(50, 20)))
    {
        Logger::clear_log_entries();
    }

    ImGui::SameLine();

    const string count_text = to_string(Logger::get_log_entries().size()) + "/" + to_string(MAX_LOG_ENTRIES);

    // Manual remaining width calculation for input text field.
    const float input_width = calculate_remaining_width(count_text);

    // Search bar
    static char search_buffer[64] = "";
    ImGui::SetNextItemWidth(input_width > 0 ? input_width : 0);
    ImGui::InputTextWithHint("##log_filter", "Filter logs by search...", search_buffer, IM_ARRAYSIZE(search_buffer));

    ImGui::SameLine();

    // Log count tracker
    ImGui::Text("%s", count_text.c_str());

    ImGuiB3D::SeparatorWithSpacing(1);

    // Log table entries
    ImGui::BeginTable("##console_text", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY);
    {
        for (const auto& log_entry : Logger::get_log_entries())
        {
            if (search_buffer[0] != '\0' && !ImGuiB3D::StringContainsIgnoreCase(log_entry, string(search_buffer)))
            {
                continue;
            }
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
