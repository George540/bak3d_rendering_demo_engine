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

#include "material.h"

#include <fstream>
#include <json.hpp>

#include "file_loader.h"
#include "resource_manager.h"
#include "shader.h"

using namespace std;

namespace
{
    glm::vec2 json_to_vec2(const nlohmann::json& v)
    {
        return { v[0].get<float>(), v[1].get<float>() };
    }

    glm::vec3 json_to_vec3(const nlohmann::json& v)
    {
        return { v[0].get<float>(), v[1].get<float>(), v[2].get<float>() };
    }

    glm::vec4 json_to_vec4(const nlohmann::json& v)
    {
        return { v[0].get<float>(), v[1].get<float>(), v[2].get<float>(), v[3].get<float>() };
    }

    glm::mat2 json_to_mat2(const nlohmann::json& v)
    {
        return glm::mat2{
            v[0].get<float>(), v[1].get<float>(),
            v[2].get<float>(), v[3].get<float>()
        };
    }

    glm::mat3 json_to_mat3(const nlohmann::json& v)
    {
        return glm::mat3{
            v[0].get<float>(), v[1].get<float>(), v[2].get<float>(),
            v[3].get<float>(), v[4].get<float>(), v[5].get<float>(),
            v[6].get<float>(), v[7].get<float>(), v[8].get<float>()
        };
    }

    glm::mat4 json_to_mat4(const nlohmann::json& v)
    {
        return glm::mat4{
            v[0].get<float>(),  v[1].get<float>(),  v[2].get<float>(),  v[3].get<float>(),
            v[4].get<float>(),  v[5].get<float>(),  v[6].get<float>(),  v[7].get<float>(),
            v[8].get<float>(),  v[9].get<float>(),  v[10].get<float>(), v[11].get<float>(),
            v[12].get<float>(), v[13].get<float>(), v[14].get<float>(), v[15].get<float>()
        };
    }
}

Material::Material(const string& path, const string& file_name, const ShaderRef& shader)
    : Asset(path, file_name)
{
    if (shader)
    {
        m_shader = shader;
    }
    else
    {
        load_from_file(path);
    }
}

Material::Material(const Material& other_material)
    : Asset(other_material.get_path(), other_material.get_file_name() + "_copy")
{
    m_shader = other_material.m_shader;
    
    m_uniform_flags = other_material.m_uniform_flags;
    m_int_uniforms = other_material.m_int_uniforms;
    m_uint_uniforms = other_material.m_uint_uniforms;
    m_float_uniforms = other_material.m_float_uniforms;
    m_vec2_uniforms = other_material.m_vec2_uniforms;
    m_vec3_uniforms = other_material.m_vec3_uniforms;
    m_vec4_uniforms = other_material.m_vec4_uniforms;
    m_mat2_uniforms = other_material.m_mat2_uniforms;
    m_mat3_uniforms = other_material.m_mat3_uniforms;
    m_mat4_uniforms = other_material.m_mat4_uniforms;
}

void Material::load_from_file(const string& path)
{
    ifstream file(path);
    if (!file.is_open())
    {
        throw runtime_error("Cannot open material: " + path);
    }

    nlohmann::json j;
    file >> j;

    const string material_name = FileLoader::get_name_from_path(path);
    const string shader_name   = j.at("shader").get<string>();

    m_shader = ResourceManager::get_shader(shader_name);

    for (auto& [uniform_name, entry] : j.at("uniforms").items())
    {
        const string type = entry.at("type").get<string>();
        const auto& val = entry.at("value");

        if      (type == "bool")  set_bool (uniform_name, val.get<bool>());
        else if (type == "int")   set_int  (uniform_name, val.get<int>());
        else if (type == "uint")  set_uint (uniform_name, val.get<unsigned int>());
        else if (type == "float") set_float(uniform_name, val.get<float>());
        else if (type == "vec2")  set_vec2 (uniform_name, json_to_vec2(val));
        else if (type == "vec3")  set_vec3 (uniform_name, json_to_vec3(val));
        else if (type == "vec4")  set_vec4 (uniform_name, json_to_vec4(val));
        else if (type == "mat2")  set_mat2 (uniform_name, json_to_mat2(val));
        else if (type == "mat3")  set_mat3 (uniform_name, json_to_mat3(val));
        else if (type == "mat4")  set_mat4 (uniform_name, json_to_mat4(val));
        else if (type == "sampler2D")
        {
            const string texture_name = val.get<string>();
            if (TextureRef texture = ResourceManager::get_texture(texture_name))
            {
                if (uniform_name == "material.diffuse")
                {
                    set_int(uniform_name, 0);
                    m_texture_names[aiTextureType_DIFFUSE] = texture_name;
                }
                else if (uniform_name == "material.specular")
                {
                    set_int(uniform_name, 1);
                    m_texture_names[aiTextureType_SPECULAR] = texture_name;
                }
                else if (uniform_name == "material.normal")
                {
                    set_int(uniform_name, 2);
                    m_texture_names[aiTextureType_HEIGHT] = texture_name;
                }
            }
        }
        else
        {
            throw runtime_error("Unknown uniform type: " + type);
        }

        apply();
    }
}

void Material::apply()
{
    if (!m_shader) return;

    m_shader->use();

    for (const auto& [name, value] : m_uniform_flags)
    {
        m_shader->set_bool(name, value);
    }

    for (const auto& [name, value] : m_uint_uniforms)
    {
        m_shader->set_uint(name, value);
    }

    for (const auto& [name, value] : m_int_uniforms)
    {
        m_shader->set_int(name, value);
    }

    for (const auto& [name, value] : m_float_uniforms)
    {
        m_shader->set_float(name, value);
    }

    for (const auto& [name, value] : m_vec2_uniforms)
    {
        m_shader->set_vec2(name, value);
    }

    for (const auto& [name, value] : m_vec3_uniforms)
    {
        m_shader->set_vec3(name, value);
    }

    for (const auto& [name, value] : m_vec4_uniforms)
    {
        m_shader->set_vec4(name, value);
    }

    for (const auto& [name, value] : m_mat2_uniforms)
    {
        m_shader->set_mat2(name, value);
    }

    for (const auto& [name, value] : m_mat3_uniforms)
    {
        m_shader->set_mat3(name, value);
    }

    for (const auto& [name, value] : m_mat4_uniforms)
    {
        m_shader->set_mat4(name, value);
    }
}

void Material::bind_textures_cache()
{
    for (const auto& [type, value] : m_texture_names)
    {
        const TextureRef texture = ResourceManager::get_texture(value);
        if (get_bool("material.use_diffuse_texture") && type == aiTextureType_DIFFUSE)
        {
            texture->bind(0);
            set_int("material.diffuse", 0);
        }
        if (get_bool("material.use_specular_texture") && type == aiTextureType_SPECULAR)
        {
            texture->bind(1);
            set_int("material.specular", 1);
        }
        if (get_bool("material.use_normal_texture") && type == aiTextureType_HEIGHT)
        {
            texture->bind(2);
            set_int("material.normal", 2);
        }
    }
}

std::string Material::get_texture_by_type(const aiTextureType& key_name)
{
    return m_texture_names[key_name];
}

bool Material::has_uniform(const std::string& key_name) const
{
    return m_uniform_flags.contains(key_name)
            || m_int_uniforms.contains(key_name)
            || m_float_uniforms.contains(key_name)
            || m_vec2_uniforms.contains(key_name)
            || m_vec3_uniforms.contains(key_name)
            || m_vec4_uniforms.contains(key_name)
            || m_mat2_uniforms.contains(key_name)
            || m_mat3_uniforms.contains(key_name)
            || m_mat4_uniforms.contains(key_name);
}
