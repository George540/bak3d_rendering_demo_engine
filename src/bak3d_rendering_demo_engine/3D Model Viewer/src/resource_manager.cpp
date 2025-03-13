﻿#include "resource_manager.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "file_loader.h"

using namespace std;

// Instantiate static variables
unordered_map<string, Shader> ResourceManager::Shaders;
unordered_map<string, Texture2D> ResourceManager::Textures;
unordered_map<string, Model> ResourceManager::Models;

void ResourceManager::initialize()
{
    initialize_shaders();
    initialize_predefined_textures();
    initialize_models();
}

void ResourceManager::initialize_shaders()
{
    auto directory = filesystem::absolute("shaders");
    auto vertex_shader_files = FileLoader::get_files_by_type_with_path(directory, vert );
    auto fragment_shader_files = FileLoader::get_files_by_type_with_path(directory, frag );

    if (vertex_shader_files.size() != fragment_shader_files.size())
    {
        throw runtime_error("ERROR::SHADER_COMPILATION_ERROR: Vertex-fragment binding miss-match!");
    }

    auto vertex_paths = FileLoader::get_vector_items_to_array(vertex_shader_files);
    auto fragment_paths = FileLoader::get_vector_items_to_array(fragment_shader_files);
    
    for (const auto shader_file_pair : vertex_shader_files)
    {
        auto shader_name = shader_file_pair.first.substr(0, shader_file_pair.first.find('.'));
        if (!Shaders.contains(shader_name))
        {
            auto vertex_shader_source = shader_file_pair.second;
            auto fragment_shader_source = 
            Shaders[shader_name] = Shader(
            string(directory.generic_string() + "/"+ shader_name + ".vert").c_str(),
            string(directory.generic_string() + "/"+ shader_name + ".frag").c_str(),
            shader_name);
        }
    }

    cout << "Number of shaders compiled: " << Shaders.size() << endl;

    if (Shaders.empty())
    {
        throw runtime_error("ERROR::SHADER_COMPILATION_ERROR: No shaders loaded!");
    }
}

void ResourceManager::initialize_predefined_textures()
{
    auto image_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/particles-textures"), png);
    for (auto [file_name, file_path] : image_files)
    {
        Textures[file_name] = Texture2D(file_path, file_name, aiTextureType_DIFFUSE, TextureUseType::Particle);
    }
}

void ResourceManager::initialize_models()
{
    auto model_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets"), obj);
    int index = 0;
    for (const auto& model_pair : model_files)
    {
        auto model_file_name = model_pair.first;
        Models[model_file_name] = Model(model_pair.second, model_file_name, index++);
    }
}

void ResourceManager::set_camera(Camera& camera)
{
    for (auto model : Models)
    {
        model.second.set_camera(camera);
    }
}


void ResourceManager::shutdown()
{
    for (const auto& val : Shaders | views::values)
    {
        glDeleteProgram(val.get_asset_id());
    }

    Shaders.clear();
    Textures.clear();
    Models.clear();
}