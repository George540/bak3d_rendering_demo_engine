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

#include "post_processor.h"

#include "vertex_array.h"
#include "Asset/resource_manager.h"
#include "Core/global_settings.h"
#include "Core/logger.h"
#include "Input/event_manager.h"

using namespace std;

namespace
{
    Shader* m_shader;
    unique_ptr<FrameBuffer> m_fbo;

    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    ElementBuffer* m_ebo;

    PostProcessColoring post_process_coloring_payload = PostProcessColoring();

    float normalize_value(const float raw_value, const float min, const float max, const float new_min = -1.0f, const float new_max = 1.0f)
    {
        return (raw_value - min) / (max - min) * (new_max - new_min) + new_min;
    }
}
void PostProcessor::initialize()
{
    m_fbo = make_unique<FrameBuffer>(0, nullptr, EventManager::get_window_width(), EventManager::get_window_height());
    auto shaders = ResourceManager::Shaders;
    m_shader = ResourceManager::get_shader("PostProcessingShader");
    create_quad();
    B3D_LOG_INFO("Post Processing initialized...");
}

void PostProcessor::shutdown()
{
    destroy_quad();
    m_shader = nullptr;
    m_fbo.reset();
    m_fbo = nullptr;
}

void PostProcessor::begin_frame()
{
    m_fbo->bind_object();
    m_shader->use();
}

void PostProcessor::process_frame(const FrameBuffer& resolved_fbo)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<int>(resolved_fbo.get_render_buffer()));
    m_shader->set_int("screenTexture", 0);

    process_post_process_coloring_payload();
    
    draw_quad();
}

void PostProcessor::end_frame()
{
    m_shader->unuse();
    m_fbo->unbind_object();
}

Shader* PostProcessor::get_shader()
{
    return m_shader;
}

FrameBuffer* PostProcessor::get_frame_buffer()
{
    return m_fbo.get();
}

void PostProcessor::resize(GLuint width, GLuint height)
{
    m_fbo->resize(width, height);
}

void PostProcessor::create_quad()
{
    m_vao = new VertexArray();
    m_vao->bind_object();

    m_vbo = new VertexBuffer(static_cast<GLsizei>(QUAD_VERTICES.size()) * VEC4_SIZE, QUAD_VERTICES.data());
    m_ebo = new ElementBuffer(static_cast<GLsizei>(QUAD_INDICES.size()) * UINT_SIZE, QUAD_INDICES.data());

    m_vao->set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, VEC4_SIZE, nullptr);
    m_vao->set_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, VEC4_SIZE, reinterpret_cast<void*>(2 * sizeof(float)));

    m_vao->unbind_object();
}

void PostProcessor::draw_quad()
{
    m_vao->bind_object();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(QUAD_INDICES.size()), GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();
}

void PostProcessor::destroy_quad()
{
    delete m_ebo;
    delete m_vbo;
    delete m_vao;
}

void PostProcessor::process_post_process_coloring_payload()
{
    post_process_coloring_payload.invert = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::PostProcessing_Coloring_Invert);
    post_process_coloring_payload.grayscale = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::PostProcessing_Coloring_Grayscale);
    post_process_coloring_payload.brightness = normalize_value(GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcessing_Coloring_Brightness), -10.0f, 10.0f);
    post_process_coloring_payload.contrast = normalize_value(GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcessing_Coloring_Contrast), -10.0f, 10.0f);
    post_process_coloring_payload.hue = normalize_value(GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcessing_Coloring_Hue), -10.0f, 10.0f);
    post_process_coloring_payload.saturation = normalize_value(GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcessing_Coloring_Saturation), -10.0f, 10.0f);
    post_process_coloring_payload.temperature = normalize_value(GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::PostProcessing_Coloring_Temperature), -10.0f, 10.0f);

    m_shader->set_bool("postProcessColoring.invert", post_process_coloring_payload.invert);
    m_shader->set_bool("postProcessColoring.grayscale", post_process_coloring_payload.grayscale);
    m_shader->set_float("postProcessColoring.brightness", post_process_coloring_payload.brightness);
    m_shader->set_float("postProcessColoring.contrast", post_process_coloring_payload.contrast);
    m_shader->set_float("postProcessColoring.hue", post_process_coloring_payload.hue);
    m_shader->set_float("postProcessColoring.saturation", post_process_coloring_payload.saturation);
    m_shader->set_float("postProcessColoring.temperature", post_process_coloring_payload.temperature);
}
