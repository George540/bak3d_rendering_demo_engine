#include "material.h"

#include "resource_manager.h"
#include "shader.h"

Material::Material(Shader* shader) : m_shader(shader) {}

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
