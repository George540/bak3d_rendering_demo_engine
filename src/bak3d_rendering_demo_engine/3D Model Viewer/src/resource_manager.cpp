#include "resource_manager.h"

#include <filesystem>
#include <iostream>
#include <ranges>

using namespace std;

// Instantiate static variables
unordered_map<string, Shader> ResourceManager::Shaders;
unordered_map<string, Texture2D> ResourceManager::Textures;

void ResourceManager::initialize()
{
    //Shaders will be precompiled before engine is initialized
    constexpr const char* shader_names[] = { "LineShader", "GridShader", "LightShader", "ModelShader", "DissectShader", "ParticleShader" };
    constexpr size_t library_size = size(shader_names);

    for (const auto i : shader_names)
    {
        auto shader_name = string(i);
        Shaders[shader_name] = Shader(
            filesystem::absolute("shaders/" + shader_name + ".vert").string().c_str(),
            filesystem::absolute("shaders/" + shader_name + ".frag").string().c_str(),
            shader_name);
    }

    cout << "Number of shaders compiled: " << Shaders.size() << endl;

    if (Shaders.empty())
    {
        throw runtime_error("ERROR::SHADER_COMPILATION_ERROR: No shaders loaded!");
    }
    if (Shaders.size() != library_size)
    {
        throw runtime_error("ERROR::SHADER_COMPILATION_ERROR: Not all shaders have been compiled!");
    }
}

void ResourceManager::shutdown()
{
    for (const auto& val : Shaders | views::values)
    {
        glDeleteProgram(val.get_id());
    }
}