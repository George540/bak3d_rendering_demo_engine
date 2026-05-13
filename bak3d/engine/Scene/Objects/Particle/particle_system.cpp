#include "particle_system.h"

#include "Asset/resource_manager.h"

#include "Core/logger.h"

using namespace std;

ParticleSystem::ParticleSystem(const string& name)
    : RenderableObject(ResourceManager::get_material("particle"),
                glm::vec3(0.0f),
                        name)
{
    // Build shared quad geometry (unit quad in 0..1 range, shader centres it)
    m_vao = new VertexArray();
    m_vao->bind_object();

    m_vbo = new VertexBuffer(static_cast<GLsizei>(QUAD_VERTICES.size()) * VEC4_SIZE, QUAD_VERTICES.data());
    m_ebo = new ElementBuffer(static_cast<GLsizei>(QUAD_INDICES.size()) * UINT_SIZE, QUAD_INDICES.data());

    // Attribute 0: quad vertex (vec4: xy=pos, zw=uv)
    m_vao->set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, VEC4_SIZE, nullptr);
    m_vao->unbind_object();

    // Add one emitter by default
    add_emitter("Emitter_1");

    B3D_LOG_INFO("Particle System '%s' created.", name.c_str());
}

ParticleSystem::~ParticleSystem()
{
    m_emitters.clear();
    m_emitter_gpu.clear();
}

void ParticleSystem::ensure_ibo_capacity(const ParticleEmitter& emitter)
{
    const string& emitter_name = emitter.get_name();
    const int needed = emitter.get_max_particles();
    auto& gpu_data = m_emitter_gpu[emitter_name];

    if (gpu_data.current_capacity < needed)
    {
        // Rebuild IBO with new size
        // Layout matches ParticleInstanceData
        const GLsizei byte_size = static_cast<GLsizei>(sizeof(ParticleInstanceData)) * needed;

        gpu_data.ibo = make_unique<InstanceBuffer>(byte_size, nullptr, GL_DYNAMIC_DRAW);
        gpu_data.current_capacity = needed;

        // Bind VAO and re-declare instanced attributes
        // (must be done each time the IBO is recreated)
        m_vao->bind_object();
        gpu_data.ibo->bind_object();

        constexpr GLsizei stride = sizeof(ParticleInstanceData);
        // location 1: position (vec3)
        m_vao->set_attrib_pointer(1, 4, GL_FLOAT, GL_FALSE, stride,
            reinterpret_cast<void*>(offsetof(ParticleInstanceData, position)), 1);
        // location 2: color (vec4)
        m_vao->set_attrib_pointer(2, 4, GL_FLOAT, GL_FALSE, stride,
            reinterpret_cast<void*>(offsetof(ParticleInstanceData, color)), 1);
        // location 3: scale (float)
        m_vao->set_attrib_pointer(3, 1, GL_FLOAT, GL_FALSE, stride,
            reinterpret_cast<void*>(offsetof(ParticleInstanceData, scale)), 1);

        m_vao->unbind_object();
    }
}

void ParticleSystem::update(const float dt)
{
    if (!m_visible)
    {
        return;
    }

    for (auto& emitter : m_emitters)
    {
        emitter->update(dt, m_position);
        ensure_ibo_capacity(*emitter);

        // Upload instance data
        auto& gpu_data = m_emitter_gpu[emitter->get_name()];
        const auto& instance_data = emitter->get_instance_data();

        gpu_data.ibo->bind_object();
        gpu_data.ibo->set_buffer_sub_data(
            instance_data.data(),
            sizeof(ParticleInstanceData) * emitter->get_max_particles(),
            0);
        gpu_data.ibo->unbind_object();
    }
}

void ParticleSystem::draw() const
{
    if (!m_visible)
    {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    (*m_material_slot)->set_int("sprite", 0);
    RenderableObject::draw();

    m_vao->bind_object();

    for (const auto& emitter : m_emitters)
    {
        if (!emitter->is_enabled() || emitter->get_alive_count() == 0)
        {
            continue;
        }

        // Bind texture
        const TextureRef& texture = emitter->get_texture();
        if (texture)
        {
            texture->bind(0);
        }

        // Draw instanced
        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(QUAD_INDICES.size()),
            GL_UNSIGNED_INT,
            nullptr,
            emitter->get_max_particles() // GPU clips dead ones via alpha = 0
        );

        emitter->draw();

        if (texture)
        {
            Texture2D::unbind();
        }
    }

    m_vao->unbind_object();

    (*m_material_slot)->get_shader()->unuse();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

ParticleEmitter* ParticleSystem::add_emitter(const string& name, const ParticleEmitterConfig& cfg)
{
    // Guard against duplicate names
    for (const auto& emitter : m_emitters)
    {
        if (emitter->get_name() == name)
        {
            B3D_LOG_WARNING("ParticleSystem: emitter '%s' already exists.", name.c_str());
            return emitter.get();
        }
    }

    auto emitter = make_unique<ParticleEmitter>(name, cfg);
    ParticleEmitter* raw = emitter.get();
    m_emitters.push_back(move(emitter));

    // Ensure GPU slot exists immediately
    m_emitter_gpu[name]; // default-constructs EmitterGPUData
    ensure_ibo_capacity(*raw);

    B3D_LOG_INFO("ParticleSystem: emitter '%s' added (%d max particles).",
                 name.c_str(), cfg.max_particles);
    return raw;
}

void ParticleSystem::remove_emitter(const string& name)
{
    const auto it = ranges::find_if(m_emitters,
                                    [&name](const unique_ptr<ParticleEmitter>& e){ return e->get_name() == name; });

    if (it == m_emitters.end())
    {
        B3D_LOG_WARNING("ParticleSystem: emitter '%s' not found for removal.", name.c_str());
        return;
    }

    m_emitters.erase(it);
    m_emitter_gpu.erase(name);
    B3D_LOG_INFO("ParticleSystem: emitter '%s' removed.", name.c_str());
}

ParticleEmitter* ParticleSystem::get_emitter(const string& name) const
{
    for (const auto& emitter : m_emitters)
    {
        if (emitter->get_name() == name)
        {
            return emitter.get();
        }
    }
    return nullptr;
}
