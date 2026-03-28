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
