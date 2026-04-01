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

#include "asset_viewer.h"

#include "imgui_b3d_extensions.h"
#include "Loader/resource_manager.h"

using namespace std;

namespace
{
    auto m_image_textures = ResourceManager::Textures;
    string m_search_name_string;
    
}

AssetPanel::AssetPanel() : EditorPanel("Assets")
{
    
}

void AssetPanel::begin_frame()
{
    EditorPanel::begin_frame();
}

void AssetPanel::update()
{
    EditorPanel::update();

    draw_asset_toolbar();

    ImGuiB3D::SeparatorWithSpacing(1);

    draw_asset_grid();
}

void AssetPanel::end_frame()
{
    EditorPanel::end_frame();
}

void AssetPanel::draw_asset_toolbar()
{
    if (ImGui::Button("Clear", ImVec2(50, 20)))
    {
        
    }

    ImGui::SameLine();
    
    static char search_buffer[64] = "";
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::InputTextWithHint("##asset_filter", "Filter assets by name...", search_buffer, IM_ARRAYSIZE(search_buffer)))
    {
        m_search_name_string = string(search_buffer);
    }
}

void AssetPanel::draw_asset_grid()
{
    
}
