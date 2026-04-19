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
    draw_post_processor_settings();
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

void Environment::draw_post_processor_settings()
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Effects"))
    {
        draw_rasterization_settings();
        draw_post_processing_settings();

        ImGui::TreePop();
    }
}

void Environment::draw_rasterization_settings()
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Rasterization"))
    {
        bool msaa_enabled = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::AA_MSAA_Enabled);
        ImGuiB3D::PropertyToggle("MSAA", &msaa_enabled, "Toggle Multisample Anti-Aliasing");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::AA_MSAA_Enabled, msaa_enabled);

        ImGui::BeginDisabled(!msaa_enabled);
        {
            int msaa_sample = GlobalSettings::get_global_setting_value<int>(GlobalSettingOption::AA_MSAA_Samples);
            const string preview = to_string(msaa_sample) + "x" + to_string(msaa_sample);
            if (ImGuiB3D::PropertyBeginDropdown("MSAA Samples", preview.c_str(), "Controls MSAA quality."))
            {
                for (int n = 0; n < m_msaa_samples.size(); n++)
                {
                    ImGui::PushID(n);

                    // Bitwise shift to power of two:
                    // Index 0 -> 2 (2^1)
                    // Index 1 -> 4 (2^2)
                    // Index 2 -> 8 (2^3)...
                    const int sample_value = 1 << (n + 1);
                    const bool is_selected = (msaa_sample == sample_value);

                    if (ImGui::Selectable(m_msaa_samples[n].c_str(), is_selected))
                    {
                        msaa_sample = sample_value;
                        GlobalSettings::set_global_setting<int>(GlobalSettingOption::AA_MSAA_Samples, msaa_sample);
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }

                    ImGui::PopID();
                }

                ImGui::EndCombo();
            }
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
        bool post_process_enabled = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::PostProcessing_Enabled);
        ImGuiB3D::PropertyToggle("Enabled", &post_process_enabled, "Toggle Post Processing.");
        GlobalSettings::set_global_setting<bool>(GlobalSettingOption::PostProcessing_Enabled, post_process_enabled);

        ImGui::BeginDisabled(!post_process_enabled);
        {
            draw_color_grading_settings();
        }
        ImGui::EndDisabled();
        
        ImGui::TreePop();
    }
}

void Environment::draw_color_grading_settings()
{
    if (ImGuiB3D::PropertyButton("Reset Defaults", "Reset Color Grading", "Reset Post Processing effects to disabled values."))
    {
        reset_color_grading_to_defaults();
    }
    
    bool invert = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::PostProcess_ColorGrading_Invert);
    ImGuiB3D::PropertyToggle("Invert", &invert, "Invert color image.");
    GlobalSettings::set_global_setting<bool>(GlobalSettingOption::PostProcess_ColorGrading_Invert, invert);

    bool grayscale = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::PostProcess_ColorGrading_Grayscale);
    ImGuiB3D::PropertyToggle("Grayscale", &grayscale, "Turn color image into black and white using Perceptive Luminance.");
    GlobalSettings::set_global_setting<bool>(GlobalSettingOption::PostProcess_ColorGrading_Grayscale, grayscale);

    float brightness = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_Brightness);
    ImGuiB3D::PropertySliderFloat("Brightness", &brightness, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust color image's brightness levels");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Brightness, brightness);

    float contrast = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_Contrast);
    ImGuiB3D::PropertySliderFloat("Contrast", &contrast, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust color image's contrast.");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Contrast, contrast);

    float hue = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_Hue);
    ImGuiB3D::PropertySliderFloat("Hue", &hue, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust color image's hue.");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Hue, hue);

    float saturation = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_Saturation);
    ImGuiB3D::PropertySliderFloat("Saturation", &saturation, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust color image's saturation.");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Saturation, saturation);

    float temperature = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_Temperature);
    ImGuiB3D::PropertySliderFloat("Temperature", &temperature, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust color image's temperature. Controls red and blue color channels.");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Temperature, temperature);

    float vignette_intensity = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcess_ColorGrading_VignetteIntensity);
    ImGuiB3D::PropertySliderFloat("Vignette Intensity", &vignette_intensity, -POST_PROCESS_COLORING_SLIDER_CLAMP, POST_PROCESS_COLORING_SLIDER_CLAMP, "%.1f", "Adjust vignette intensity. Positive intensity gives a darker vignette tone and negative intensity colors the inverse.");
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_VignetteIntensity, vignette_intensity);

    glm::vec4 vignette_color = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::PostProcess_ColorGrading_VignetteColor);
    float bg_col[4] = {  vignette_color.r,  vignette_color.g,  vignette_color.b, vignette_color.a };
    ImGuiB3D::PropertyColorPicker("Color", bg_col, "Adjust vignette coloring. Color gets inverted when intensity is negative.");
    vignette_color.r = bg_col[0];
    vignette_color.g = bg_col[1];
    vignette_color.b = bg_col[2];
    vignette_color.a = bg_col[3];
    GlobalSettings::set_global_setting<glm::vec4>(GlobalSettingOption::PostProcess_ColorGrading_VignetteColor, vignette_color);
    GlobalSettings::set_global_setting<glm::vec4>(GlobalSettingOption::PostProcess_ColorGrading_VignetteColor, vignette_color);
}

void Environment::reset_color_grading_to_defaults()
{
    GlobalSettings::set_global_setting<bool>(GlobalSettingOption::PostProcess_ColorGrading_Invert, false);
    GlobalSettings::set_global_setting<bool>(GlobalSettingOption::PostProcess_ColorGrading_Grayscale, false);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Brightness, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Contrast, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Hue, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Saturation, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_Temperature, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_VignetteIntensity, 0.0f);
    GlobalSettings::set_global_setting<float>(GlobalSettingOption::PostProcess_ColorGrading_VignetteColor, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}
