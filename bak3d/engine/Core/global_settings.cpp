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

#include "global_settings.h"

std::map<GlobalSettingOption, GlobalSettingValueType> GlobalSettings::global_settings = {};

// TODO: to be used for property labels in ImGui
const char* GlobalSettings::to_string(GlobalSettingOption enum_setting)
{
    switch (enum_setting)
    {
        case GlobalSettingOption::GridRendering: return "Enable Grid";
        case GlobalSettingOption::AxisRendering: return "Enable Axis";
        case GlobalSettingOption::BackgroundColor: return "Background Color";
        case GlobalSettingOption::Light_Enabled: return "Enabled";
        case GlobalSettingOption::Light_HorizontalRotation: return "Horizontal Rotation";
        case GlobalSettingOption::Light_VerticalRotation: return "Vertical Rotation";
        case GlobalSettingOption::Light_Scaling: return "Scaling";
        case GlobalSettingOption::Light_OriginDistance: return "Origin Distance";
        case GlobalSettingOption::Light_Color: return "Color";
        case GlobalSettingOption::Light_Intensity: return "Intensity";
        case GlobalSettingOption::MSAA_Enabled: return "MSAA Enabled";
        case GlobalSettingOption::MSAA_Samples: return "MSAA Samples";
        case GlobalSettingOption::PostProcessing_Enabled: return "Enabled";
        case GlobalSettingOption::PostProcessing_Coloring_Invert: return "Invert";
        case GlobalSettingOption::PostProcessing_Coloring_Brightness: return "Brightness";
        case GlobalSettingOption::PostProcessing_Coloring_Contrast: return "Contrast";
        case GlobalSettingOption::PostProcessing_Coloring_Grayscale: return "Grayscale";
        case GlobalSettingOption::PostProcessing_Coloring_Hue: return "Hue";
        case GlobalSettingOption::PostProcessing_Coloring_Saturation: return "Saturation";
        case GlobalSettingOption::PostProcessing_Coloring_Temperature: return "Temperature";
        case GlobalSettingOption::Max: return "Max";
        default: return "Unknown";
    }
}

void GlobalSettings::initialize()
{
    global_settings.clear();
    global_settings[GlobalSettingOption::GridRendering] = true;
    global_settings[GlobalSettingOption::AxisRendering] = true;
    global_settings[GlobalSettingOption::BackgroundColor] = glm::vec4(0.133f, 0.168f, 0.2f, 1.0f);
    global_settings[GlobalSettingOption::Light_Enabled] = true;
    global_settings[GlobalSettingOption::Light_HorizontalRotation] = 45.0f;
    global_settings[GlobalSettingOption::Light_VerticalRotation] = 135.0f;
    global_settings[GlobalSettingOption::Light_Scaling] = 0.5f;
    global_settings[GlobalSettingOption::Light_OriginDistance] = 5.0f;
    global_settings[GlobalSettingOption::Light_Color] = glm::vec4(1.0f);
    global_settings[GlobalSettingOption::Light_Intensity] = 1.0f;
    global_settings[GlobalSettingOption::MSAA_Enabled] = true;
    global_settings[GlobalSettingOption::MSAA_Samples] = 4; // turns to 4x4 when passed to GLFW
    global_settings[GlobalSettingOption::PostProcessing_Enabled] = false;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Invert] = false;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Grayscale] = false;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Brightness] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Contrast] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Hue] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Saturation] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_Temperature] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_VignetteIntensity] = 0.0f;
    global_settings[GlobalSettingOption::PostProcessing_Coloring_VignetteColor] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void GlobalSettings::shutdown()
{
    global_settings.clear();
}

void GlobalSettings::reset_to_defaults()
{
    initialize();
}
