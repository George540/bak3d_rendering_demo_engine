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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <unordered_map>

class Shader;
class Texture2D;
class Model;
class Camera;
class Light;
class Material;

/*
 * Static class for managing loaded rendering resources such as shaders, textures, materials
 * and models to be used in the engine.
 */
class ResourceManager
{
public:
    // resource storage
    static std::unordered_map<std::string, Shader*> Shaders;
    static std::unordered_map<std::string, Texture2D*> Textures;
    static std::unordered_map<std::string, Material*> Materials;
    static std::unordered_map<std::string, Model*> Models;

    static void initialize();
    static void set_camera(Camera& camera, Light& light);
    
    static Shader* get_shader(const std::string& shader_name) { return Shaders[shader_name]; }
    
    static void add_texture(const std::string& texture_name, Texture2D* texture) { Textures[texture_name] = texture; }
    static Texture2D* get_texture(const std::string& texture_name) { return Textures[texture_name]; }
    
    static Model* get_model(const std::string& model_name) { return Models[model_name]; }

    static void add_material(const std::string& material_name, Material* material) { Materials[material_name] = material; }
    static Material* get_material(const std::string& material_name) { return Materials[material_name]; }
    
    static void shutdown();
private:
    static void initialize_shaders();
    static void initialize_predefined_textures();
    static void initialize_models();
    
    ResourceManager() = default;
};

#endif