#include "resource_manager.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "file_loader.h"

using namespace std;

// Instantiate static variables
unordered_map<string, Shader> ResourceManager::Shaders;
unordered_map<string, Texture2D> ResourceManager::Textures;

void ResourceManager::initialize()
{
    initialize_shaders();
    initialize_predefined_textures();
    initialize_models();
}

void ResourceManager::initialize_shaders()
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

void ResourceManager::initialize_predefined_textures()
{
    auto image_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/particles-textures"), FileType::png);
    for (auto [file_name, file_path] : image_files)
    {
        auto texture_name = file_name.substr(0, texture_name.find('.'));
        Textures[texture_name] = Texture2D(file_path, aiTextureType_DIFFUSE);
    }
}


void ResourceManager::initialize_models()
{
    auto model_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets"), obj);
    int index = 1;
    for (const auto& model_pair : model_files)
    {
        auto model_file_name = model_pair.first;
        auto model_name = model_file_name.substr(0, model_file_name.find('.'));
        Models[model_file_name] = Model(model_pair.second, model_file_name, model_name, index++);
    }
}

void ResourceManager::shutdown()
{
    for (const auto& val : Shaders | views::values)
    {
        glDeleteProgram(val.get_id());
    }
}