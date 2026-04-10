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
#include "Renderer/renderer.h"

using namespace std;

namespace
{
    vector<string> m_msaa_samples = { };
}

Environment::Environment() : EditorPanel("Environment")
{
    // Dynamically create MSAA sampling options for dropdown selection based on hardware's max samples.
    const int msaa_max_samples = Renderer::get_msaa_frame_buffer()->get_samples();
    m_msaa_samples.reserve(msaa_max_samples);
    for (int sample_id = 2; sample_id <= msaa_max_samples; sample_id *= 2)
    {
        string label = to_string(sample_id) + "x" + to_string(sample_id);
        m_msaa_samples.push_back(label);
    }
}

void Environment::begin_frame()
{
    EditorPanel::begin_frame();
}

void Environment::update()
{
    EditorPanel::update();

    draw_general_settings();
    draw_light_settings();
    draw_post_processing_settings();
}

void Environment::end_frame()
{
    EditorPanel::end_frame();
}

void Environment::draw_general_settings()
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("General"))
    {
        // Grid Rendering
        bool grid_rendering = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::GridRendering);
        ImGuiB3D::PropertyToggle("Render Grid", &grid_rendering, "Render scene grid in viewport");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::GridRendering, grid_rendering);

        // Axis Rendering
        bool axis_rendering = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::AxisRendering);
        ImGuiB3D::PropertyToggle("Render Axis", &axis_rendering, "Render scene axis in viewport");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::AxisRendering, axis_rendering);

        // Toggle background color
        glm::vec4 bg_color_vec4 = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::BackgroundColor);
        float bg_col[4] = {  bg_color_vec4.r,  bg_color_vec4.g,  bg_color_vec4.b, bg_color_vec4.a };
        ImGuiB3D::PropertyColorPicker("Background Color", bg_col, "Change background color using glClearColor(...)");
        bg_color_vec4.r = bg_col[0];
        bg_color_vec4.g = bg_col[1];
        bg_color_vec4.b = bg_col[2];
        bg_color_vec4.a = bg_col[3];
        GlobalSettings::set_global_setting<glm::vec4>(GlobalSettingOption::BackgroundColor, bg_color_vec4);

        ImGui::TreePop();
    }
}

void Environment::draw_light_settings()
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Point Light"))
    {
        bool light_enabled = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::Light_Enabled);
        ImGuiB3D::PropertyToggle("Enabled", &light_enabled, "Enable light in viewport");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::Light_Enabled, light_enabled);

        ImGui::BeginDisabled(!light_enabled);
        {
            // Horizontal Rotation
            float light_horizontal_rotation = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_HorizontalRotation);
            ImGuiB3D::PropertySliderFloat("Horizontal Rotation", &light_horizontal_rotation, 0.0f, 360.0f, "%.3f", "Orbit light around object horizontally.");
            GlobalSettings::set_global_setting<bool>(GlobalSettingOption::Light_HorizontalRotation, light_horizontal_rotation);

            // Vertical Rotation
            float light_vertical_rotation = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_VerticalRotation);
            ImGuiB3D::PropertySliderFloat("Vertical Rotation", &light_vertical_rotation, 0.0f, 360.0f, "%.3f", "Orbit light around object vertically.");
            GlobalSettings::set_global_setting<bool>(GlobalSettingOption::Light_VerticalRotation, light_vertical_rotation);

            // Sprite Scaling
            float sprite_scaling = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Scaling);
            ImGuiB3D::PropertySliderFloat("Sprite Scaling", &sprite_scaling, 0.1f, 3.0f, "%.3f", "Change the light's icon size.");
            GlobalSettings::set_global_setting<bool>(GlobalSettingOption::Light_Scaling, sprite_scaling);

            // Distance Offset
            float light_origin_distance = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_OriginDistance);
            ImGuiB3D::PropertySliderFloat("Distance", &light_origin_distance, 2.0f, 10.0f, "%.3f", "Light distance from origin (center of axis).");
            GlobalSettings::set_global_setting<float>(GlobalSettingOption::Light_OriginDistance, light_origin_distance);

            // Intensity
            float light_intensity = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Intensity);
            ImGuiB3D::PropertySliderFloat("Intensity", &light_intensity, 0.0f, 10.0f, "%.3f", "Intensity of the light");
            GlobalSettings::set_global_setting<float>(GlobalSettingOption::Light_Intensity, light_intensity);

            // Color
            glm::vec4 light_color = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::Light_Color);
            float bg_col[4] = {  light_color.r,  light_color.g,  light_color.b, light_color.a };
            ImGuiB3D::PropertyColorPicker("Color", bg_col, "Change light's color");
            light_color.r = bg_col[0];
            light_color.g = bg_col[1];
            light_color.b = bg_col[2];
            light_color.a = bg_col[3];
            GlobalSettings::set_global_setting<glm::vec4>(GlobalSettingOption::Light_Color, light_color);
        }
        ImGui::EndDisabled();

        ImGui::TreePop();
    }
}

void Environment::draw_post_processing_settings()
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Post Processing"))
    {
        bool msaa_enabled = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::MSAA_Enabled);
        ImGuiB3D::PropertyToggle("MSAA", &msaa_enabled, "Toggle Multisample Anti-Aliasing");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::MSAA_Enabled, msaa_enabled);

        ImGui::BeginDisabled(!msaa_enabled);
        {
            int msaa_sample = GlobalSettings::get_global_setting_value<int>(GlobalSettingOption::MSAA_Samples);
            if (ImGui::BeginCombo("MSAA Samples", (to_string(msaa_sample) + "x" + to_string(msaa_sample)).c_str())) 
            {
                for (int n = 0; n < m_msaa_samples.size(); n++) 
                {
                    // Bitwise shift to power of two:
                    // Index 0 -> 2 (2^1)
                    // Index 1 -> 4 (2^2)
                    // Index 2 -> 8 (2^3)...
                    const int sample_value = 1 << (n + 1);

                    const bool is_selected = (msaa_sample == sample_value);
                    if (ImGui::Selectable(m_msaa_samples[n].c_str(), is_selected)) 
                    {
                        msaa_sample = sample_value;
                        GlobalSettings::set_global_setting<int>(GlobalSettingOption::MSAA_Samples, msaa_sample);
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::EndDisabled();

        ImGui::TreePop();
    }
}
