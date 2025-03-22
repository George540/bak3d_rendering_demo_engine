#pragma once

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <unordered_map>

#include "shader.h"
#include "texture.h"
#include "model.h"

class ResourceManager
{
public:
    // resource storage
    static std::unordered_map<std::string, Shader*> Shaders;
    static std::unordered_map<std::string, Texture2D*> Textures;
    static std::unordered_map<std::string, Model*> Models;

    static void initialize();
    static void set_camera(Camera& camera, Light& light);
    
    static Shader* get_shader(const std::string& shader_name) { return Shaders[shader_name]; }
    
    static void add_texture(const std::string name, Texture2D* texture) { Textures[name] = texture; }
    static Texture2D* get_texture(const std::string& texture_name) { return Textures[texture_name]; }
    
    static Model* get_model(const std::string& model_name) { return Models[model_name]; }
    
    static void shutdown();
private:
    static void initialize_shaders();
    static void initialize_predefined_textures();
    static void initialize_models();
    
    ResourceManager() = default;
};

#endif