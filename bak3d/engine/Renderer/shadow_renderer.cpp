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

#include "shadow_renderer.h"

#include "debug_scope.h"
#include "Asset/resource_manager.h"
#include "Scene/scene_manager.h"

using namespace std;

unique_ptr<ShadowMapFrameBuffer> ShadowRenderer::m_shadow_fbo;
unique_ptr<ShaderStorageBuffer> ShadowRenderer::m_shadow_ssbo;
vector<const Light*> ShadowRenderer::m_light_casters;
vector<ShadowGPUData> ShadowRenderer::m_shadow_data;
GLuint ShadowRenderer::m_max_layers = 1;
ShadowSettings ShadowRenderer::m_settings;
GLuint ShadowRenderer::m_preview_view = 0;

void ShadowRenderer::initialize()
{
    GLint hardware_layers = 1;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &hardware_layers);
    m_max_layers = static_cast<GLuint>(hardware_layers);
    if (m_settings.max_casters > 0)
    {
        m_max_layers = glm::min(m_max_layers, m_settings.max_casters);
    }

    const GLuint layers = glm::clamp(m_settings.initial_layers, 1u, m_max_layers);
    m_shadow_fbo = make_unique<ShadowMapFrameBuffer>(m_settings.resolution, layers, "ShadowMap");
    m_shadow_ssbo = make_unique<ShaderStorageBuffer>(0, nullptr, SHADOW_DATA_SSBO_BINDING, GL_DYNAMIC_DRAW);
}

void ShadowRenderer::shutdown()
{
    if (m_preview_view != 0)
    {
        glDeleteTextures(1, &m_preview_view); m_preview_view = 0;
    }

    m_light_casters.clear();
    m_shadow_data.clear();
    m_shadow_fbo.reset();
    m_shadow_ssbo.reset();
}

void ShadowRenderer::render()
{
    update_and_upload_data();

    if (m_light_casters.empty())
    {
        return;
    }

    const ShaderRef shader = ResourceManager::get_shader("shadow_depth");

    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glActiveTexture(GL_TEXTURE0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(m_settings.cull_front_faces ? GL_FRONT : GL_BACK);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(m_settings.polygon_offset_factor, m_settings.polygon_offset_units);

    shader->use();
    for (size_t layer = 0; layer < m_light_casters.size(); ++layer)
    {
        DebugScopeGroup scope("Shadow Layer");

        m_shadow_fbo->bind_layer(static_cast<GLuint>(layer));
        glClear(GL_DEPTH_BUFFER_BIT);

        const glm::mat4& light_space = m_shadow_data[layer].light_space_matrix;
        shader->set_mat4("light_space_matrix", light_space);
        const Frustum light_frustum(light_space);

        for (const Mesh* mesh : SceneManager::get_current_scene()->get_all_meshes())
        {
            if (!mesh->is_visible() || !mesh->has_mesh())
            {
                continue;
            }
            const auto* data = dynamic_cast<const MeshData*>(mesh->get_mesh().operator->());
            if (!data)
            {
                continue;
            }

            // Per-light culling with the mesh's world-space bounding sphere
            const glm::mat4 model = mesh->transform.get_global_model_matrix();
            if (!data->get_aabb().is_on_frustum(light_frustum, model))
            {
                continue;
            }

            shader->set_mat4("model", model);
            data->draw();
        }
    }
    shader->unuse();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glCullFace(GL_BACK);
    m_shadow_fbo->unbind();

    // Leave the array bound for the forward and deferred passes
    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_shadow_fbo->get_depth_texture());
    glActiveTexture(GL_TEXTURE0);
}

int ShadowRenderer::get_shadow_index(const Light* light)
{
    const auto it = ranges::find(m_light_casters, light);
    return it == m_light_casters.end() ? -1 : static_cast<int>(it - m_light_casters.begin());
}

GLuint ShadowRenderer::get_light_preview_texture(const Light* light)
{
    const int layer = get_shadow_index(light);
    const GLuint source = get_shadow_map_texture();
    if (layer < 0 || source == 0)
    {
        return 0;
    }

    if (m_preview_view != 0)
    {
        glDeleteTextures(1, &m_preview_view);
    }
    glGenTextures(1, &m_preview_view);
    glTextureView(m_preview_view, GL_TEXTURE_2D, source, GL_DEPTH_COMPONENT32F, 0, 1, static_cast<GLuint>(layer), 1);

    glBindTexture(GL_TEXTURE_2D, m_preview_view);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, SWIZZLE_MASK);
    glBindTexture(GL_TEXTURE_2D, 0);

    return m_preview_view;
}

void ShadowRenderer::select_casters(const vector<const Light*>& visible_lights, const glm::vec3& camera_position)
{
    m_light_casters.clear();
    m_shadow_data.clear();

    if (!m_settings.cast_shadows)
    {
        return;
    }

    const ShaderRef shader = ResourceManager::get_shader("shadow_depth");
    if (!shader || !shader->is_shader_compiled())
    {
        return;
    }

    struct Candidate { const Light* light; float score; };
    vector<Candidate> candidates;
    for (const Light* light : visible_lights)
    {
        if (!light->get_cast_shadows())
        {
            continue;
        }
        if (light->get_type() == LightType::Directional)
        {
            candidates.push_back({ light, FLT_MAX });
        }
        else if (light->get_type() == LightType::Spot)
        {
            const float distance = glm::distance(camera_position, light->transform.get_global_position());
            candidates.push_back( {.light = light, .score = light->get_intensity() * light->get_effective_radius() / (1.0f + distance) } );
        }
    }

    // Prioritize only when the hardware/user layer limit is exceeded
    if (candidates.size() > m_max_layers)
    {
        ranges::partial_sort(candidates, candidates.begin() + m_max_layers, [](const Candidate& a, const Candidate& b) { return a.score > b.score; });
        candidates.resize(m_max_layers);
    }

    ensure_layer_capacity(candidates.size());

    m_light_casters.reserve(candidates.size());
    m_shadow_data.reserve(candidates.size());
    for (const Candidate& candidate : candidates)
    {
        m_light_casters.push_back(candidate.light);
        m_shadow_data.push_back(build_shadow_data(*candidate.light));
    }
}

void ShadowRenderer::reset_settings()
{
    constexpr ShadowSettings defaults{};
    m_settings = defaults;
    apply_resolution(defaults.resolution);
}

void ShadowRenderer::apply_resolution(GLuint resolution)
{
    m_settings.resolution = resolution;
    if (resolution == m_shadow_fbo->get_width())
    {
        return;
    }
    // Keep the layer count so no caster is lost
    m_shadow_fbo = make_unique<ShadowMapFrameBuffer>(resolution, m_shadow_fbo->get_layer_count(), "ShadowMap");
}

ShadowGPUData ShadowRenderer::build_shadow_data(const Light& light)
{
    const ShadowSettings& settings = m_settings;
    const glm::vec3 direction = glm::normalize(light.get_direction());
    const glm::vec3 up = glm::abs(direction.y) > 0.99f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 eye;
    glm::mat4 projection;
    float far_plane;
    float frustum_scale;
    float is_perspective;
    float light_size;

    if (light.get_type() == LightType::Directional)
    {
        far_plane = settings.directional_depth_range;
        frustum_scale = settings.directional_extent;
        is_perspective = 0.0f;
        light_size = glm::tan(glm::radians(settings.directional_angular_size_degrees * 0.5f));
        eye = settings.directional_focus - direction * (far_plane * 0.5f); // box centered on focus along the light axis
        projection = glm::ortho(-frustum_scale, frustum_scale, -frustum_scale, frustum_scale, settings.near_plane, far_plane);
    }
    else // Spotlight for now, others will follow
    {
        // Cone half-angle = outer angle + cone size (see Light::recompute_cone_cutoffs)
        const float half_angle = light.get_cone_angle_outer_cutoff() + light.get_cone_size() + settings.spot_fov_margin_degrees;
        const float fov = glm::radians(glm::min(half_angle * 2.0f, 170.0f));
        far_plane = glm::min(light.get_effective_radius(), settings.spot_max_range);
        frustum_scale = glm::tan(fov * 0.5f);
        is_perspective = 1.0f;
        light_size = settings.spot_light_radius;
        eye = light.transform.get_global_position();
        projection = glm::perspective(fov, 1.0f, settings.near_plane, far_plane);
    }

    const int flags = (settings.pcss_enabled ? 1 : 0) | (settings.receiver_plane_bias ? 2 : 0);

    ShadowGPUData data;
    data.light_space_matrix = projection * glm::lookAt(eye, eye + direction, up);
    data.bias_params = glm::vec4(settings.depth_bias_texels, settings.slope_bias_texels, settings.normal_offset_texels, static_cast<float>(settings.pcf_radius));
    data.projection_params = glm::vec4(settings.near_plane, far_plane, frustum_scale, is_perspective);
    data.map_params = glm::vec4(static_cast<float>(settings.resolution), 1.0f, light_size, settings.max_filter_radius_texels);
    data.pcss_params = glm::vec4(
        static_cast<float>(glm::clamp(settings.blocker_samples, 1, 64)),
        static_cast<float>(glm::clamp(settings.filter_samples, 1, 128)),
        static_cast<float>(flags),
        0.0f);
    return data;
}

void ShadowRenderer::ensure_layer_capacity(size_t needed)
{
    const GLuint current = m_shadow_fbo->get_layer_count();
    if (needed <= current)
    {
        return;
    }
    const GLuint next = glm::min(glm::max(static_cast<GLuint>(needed), current * 2), m_max_layers);
    m_shadow_fbo->resize_layers(next);
}

void ShadowRenderer::update_and_upload_data()
{
    if (m_shadow_data.empty())
    {
        return;
    }

    const auto size = static_cast<GLsizeiptr>(m_shadow_data.size()) * SHADOW_GPU_DATA_SIZE;
    m_shadow_ssbo->bind();
    m_shadow_ssbo->bind_buffer_data(m_shadow_data.data(), size);
    m_shadow_ssbo->bind_to_binding_point(SHADOW_DATA_SSBO_BINDING);
}
