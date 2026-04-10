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

const char* GlobalSettings::to_string(GlobalSettingOption enum_setting)
{
    switch (enum_setting)
    {
        case GlobalSettingOption::GridRendering: return "Grid Rendering";
        case GlobalSettingOption::AxisRendering: return "Axis Rendering";
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
        case GlobalSettingOption::Max: return "Max";
        default: return "Unknown";
    }
}

void GlobalSettings::initialize()
{
    global_settings.clear();
    global_settings[GlobalSettingOption::GridRendering] = true;
    global_settings[GlobalSettingOption::AxisRendering] = true;
    global_settings[GlobalSettingOption::BackgroundColor] = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    global_settings[GlobalSettingOption::Light_Enabled] = true;
    global_settings[GlobalSettingOption::Light_HorizontalRotation] = 45.0f;
    global_settings[GlobalSettingOption::Light_VerticalRotation] = 135.0f;
    global_settings[GlobalSettingOption::Light_Scaling] = 0.5f;
    global_settings[GlobalSettingOption::Light_OriginDistance] = 5.0f;
    global_settings[GlobalSettingOption::Light_Color] = glm::vec4(1.0f);
    global_settings[GlobalSettingOption::Light_Intensity] = 1.0f;
    global_settings[GlobalSettingOption::MSAA_Enabled] = true;
    global_settings[GlobalSettingOption::MSAA_Samples] = 4; // turns to 4x4 when passed to GLFW
}

void GlobalSettings::shutdown()
{
    global_settings.clear();
}

void GlobalSettings::reset_to_defaults()
{
    initialize();
}
