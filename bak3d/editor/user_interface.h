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

#pragma once

#ifndef USER_INTERFACE
#define USER_INTERFACE

#include <list>

#include "../engine/Scene/Objects/model.h"
#include "Scene/Objects/particle_generator.h"

struct CurrentModelInfo
{
    Model* current_model;
    std::string model_file_name;
    std::string model_file_path;
    int model_combo_index;

    CurrentModelInfo() : current_model(nullptr), model_file_name("None"), model_file_path("none"), model_combo_index(0) {}

    void set_info(std::string name, std::string path, int index)
    {
        model_file_name = name;
        model_file_path = path;
        model_combo_index = index;
    };
};

/*
 * @todo TO BE DEPRECATED AND REMADE:
 * General UI static class for modifying different object and OpenGL settings.
 * Acts as a translation layer between the user, the application and the hardware.
 */
class UserInterface
{
private:
    static void initialize_imgui();
    static void draw_viewport_window();
    static void render_metrics_window();
    static void render_object_window();
    static void render_environment_window();
public:
    static void initialize();
    static void shutdown();

    static void begin_frame();
    static void end_frame();

    static void render_demo_window();
    
    static const char* map_combo_items[];
    static int render_current;
    static bool is_full_render_selected;
    static bool is_diffuse_render_selected;
    static bool is_specular_selected;
    static bool is_normal_map_selected;
    static bool is_gamma_enabled;
    static float shininess;

    static const char* object_combo_items[];
    static int object_current;
    
    static std::vector<const char*> model_combo_items;
    static Model* model_current_object;
    static int model_current_index;

    static ParticleSystem* current_particle_system;
    static particle_info particle_payload_info;
    static std::list<std::pair<std::string, std::string>> particle_image_combo_items_list;
    static std::vector<const char*> particle_image_combo_items;
    static int particle_image_current;

    static bool is_grid_rendering;
    static glm::vec3 background_color;
    static Light* environment_point_light;
    static float light_horizontal_rotation;
    static float light_vertical_rotation;
    static float light_origin_distance;
    static glm::vec3 light_diffuse_color;
    static float light_intensity;
};

#endif