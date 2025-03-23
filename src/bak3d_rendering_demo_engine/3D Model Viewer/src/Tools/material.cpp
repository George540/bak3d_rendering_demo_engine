#include "material.h"

#include "resource_manager.h"
#include "shader.h"

Material::Material(Shader* shader) : m_shader(shader) {}

Material::Material(Shader* shader, const std::vector<std::string>& textures) : m_shader(shader)
{
    for (const auto& texture_string : textures)
    {
        Texture2D* texture = ResourceManager::get_texture(texture_string);
        
        if (texture->get_texture_type() == aiTextureType_DIFFUSE && !m_textures.contains(aiTextureType_DIFFUSE))
        {
            m_textures[aiTextureType_DIFFUSE] = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_SPECULAR && !m_textures.contains(aiTextureType_SPECULAR))
        {
            m_textures[aiTextureType_SPECULAR] = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_NORMALS && !m_textures.contains(aiTextureType_NORMALS))
        {
            m_textures[aiTextureType_NORMALS] = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_HEIGHT && !m_textures.contains(aiTextureType_HEIGHT))
        {
            m_textures[aiTextureType_HEIGHT] = texture;
        }
    }

    // Ensure unassigned texture types have a default one to be used in the shader
    if (!m_textures.contains(aiTextureType_DIFFUSE)) m_textures[aiTextureType_DIFFUSE] = ResourceManager::get_texture("None.jpg");
    if (!m_textures.contains(aiTextureType_SPECULAR)) m_textures[aiTextureType_SPECULAR] = ResourceManager::get_texture("None.jpg");
    if (!m_textures.contains(aiTextureType_NORMALS)) m_textures[aiTextureType_NORMALS] = ResourceManager::get_texture("None.jpg");
    if (!m_textures.contains(aiTextureType_HEIGHT)) m_textures[aiTextureType_HEIGHT] = ResourceManager::get_texture("None.jpg");
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

    for (const auto& [texture_type, texture_asset] : m_textures)
    {
        if (texture_type == aiTextureType_DIFFUSE)
        {
            m_shader->set_int("material.diffuse", static_cast<int>(texture_asset->get_asset_id()));
        }
        else if (texture_type == aiTextureType_SPECULAR)
        {
            m_shader->set_int("material.specular", static_cast<int>(texture_asset->get_asset_id()));
        }
        else if (texture_type == aiTextureType_NORMALS)
        {
            m_shader->set_int("material.normal", static_cast<int>(texture_asset->get_asset_id()));
        }
        else if (texture_type == aiTextureType_HEIGHT)
        {
            m_shader->set_int("material.height", static_cast<int>(texture_asset->get_asset_id()));
        }
        
        texture_asset->bind();
    }
}
