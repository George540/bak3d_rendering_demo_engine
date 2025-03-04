#pragma once

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <unordered_map>

#include "shader.h"
#include "texture.h"

class ResourceManager
{
public:
    // resource storage
    static std::unordered_map<std::string, Shader> Shaders;
    static std::unordered_map<std::string, Texture2D> Textures;

    static void initialize();
    
    static Shader* get_shader(const std::string& shader_name) { return &Shaders[shader_name]; }
    
    static Texture2D load_texture(const char *file, std::string name);
    static void add_texture(const std::string name, const Texture2D texture) { Textures[name] = texture; }
    static Texture2D* get_texture(std::string name) { return &Textures[name]; };
    
    static void shutdown();
private:
    ResourceManager() = default;
};

#endif