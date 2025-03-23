#include "resource_manager.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "file_loader.h"
#include "model.h"
#include "shader.h"
#include "texture.h"

using namespace std;

// Instantiate static variables
unordered_map<string, Shader*> ResourceManager::Shaders;
unordered_map<string, Texture2D*> ResourceManager::Textures;
unordered_map<string, Material*> ResourceManager::Materials;
unordered_map<string, Model*> ResourceManager::Models;

void ResourceManager::initialize()
{
    initialize_shaders();
    initialize_predefined_textures();
    initialize_models();
}

void ResourceManager::initialize_shaders()
{
    // Get directory and files
    auto directory = filesystem::absolute("shaders");
    auto vertex_shader_files = FileLoader::get_files_by_type_with_path(directory, vert );
    auto fragment_shader_files = FileLoader::get_files_by_type_with_path(directory, frag );

    if (vertex_shader_files.size() != fragment_shader_files.size())
    {
        throw runtime_error("ERROR::SHADER_COMPILATION_ERROR: Vertex-fragment binding miss-match!");
    }

    // Get absolute paths for both vert and frag files
    auto vertex_paths = FileLoader::get_vector_items_to_array(vertex_shader_files);
    auto fragment_paths = FileLoader::get_vector_items_to_array(fragment_shader_files);

    // Combine vert and frag paths in pairs
    vector<pair<char*, char*>> vert_frag_sources;
    vert_frag_sources.reserve(vertex_shader_files.size());
    for (int i = 0; i < vertex_shader_files.size(); ++i)
    {
        vert_frag_sources.emplace_back(vertex_paths[i], fragment_paths[i]);
    }

    // Create and bind shaders.
    for (const auto shader_file_pair : vert_frag_sources)
    {
        auto vert_path_string = string(shader_file_pair.first);
        auto shader_name = vert_path_string.substr(vert_path_string.find_last_of('/') + 1);
        shader_name = shader_name.substr(0, shader_name.find('.'));
        if (!Shaders.contains(shader_name))
        {
            Shaders[shader_name] = new Shader(
            shader_file_pair.first,
            shader_file_pair.second,
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
        Textures[file_name] = new Texture2D(file_path, file_name, aiTextureType_DIFFUSE, TextureUseType::Particle);
    }

    image_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/none"), jpg);
    for (auto [file_name, file_path] : image_files)
    {
        Textures[file_name] = new Texture2D(file_path, file_name, aiTextureType_DIFFUSE, TextureUseType::Model);
    }
}

void ResourceManager::initialize_models()
{
    auto model_files = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets"), obj);
    int index = 1;
    for (const auto& model_pair : model_files)
    {
        auto model_file_name = model_pair.first;
        Models[model_file_name] = new Model(model_pair.second, model_file_name, index++);
    }
}

void ResourceManager::set_camera(Camera& camera, Light& light)
{
    for (auto model : Models)
    {
        model.second->set_camera_and_light(camera, light);
    }
}

void ResourceManager::shutdown()
{
    for (const auto& val : Shaders | views::values)
    {
        glDeleteProgram(val->get_asset_id());
    }

    Shaders.clear();
    Textures.clear();
    Models.clear();
    Materials.clear();
}