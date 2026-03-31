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

#include "environment.h"

#include <glm/vec3.hpp>

#include "imgui_b3d_extensions.h"
#include "Core/global_settings.h"

Environment::Environment() : EditorPanel("Environment")
{
    m_flags |= ImGuiStyleVar_WindowPadding;
}

void Environment::begin_frame()
{
    EditorPanel::begin_frame();
}

void Environment::update()
{
    EditorPanel::update();

    bool grid_rendering = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::GridRendering);
    ImGuiB3D::PropertyToggle("Render Grid", &grid_rendering, "Render scene grid in viewport");
    GlobalSettings::set_global_setting<bool>(GlobalSettingOption::GridRendering, grid_rendering);

    // Toggle light color
    glm::vec4 bg_color_vec4 = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::BackgroundColor);
    float bg_col[4] = {  bg_color_vec4.r,  bg_color_vec4.g,  bg_color_vec4.b, bg_color_vec4.a };
    //ImGui::ColorEdit4("Background Color", bg_col);
    ImGuiB3D::PropertyColorPicker("Background Color", bg_col, "Change background color using glClearColor(...)");
    bg_color_vec4.r = bg_col[0];
    bg_color_vec4.g = bg_col[1];
    bg_color_vec4.b = bg_col[2];
    bg_color_vec4.a = bg_col[3];
    GlobalSettings::set_global_setting<glm::vec4>(GlobalSettingOption::BackgroundColor, bg_color_vec4);
}

void Environment::end_frame()
{
    EditorPanel::end_frame();
}
