#include <iostream>
#include <random>

#include "particle_generator.h"
#include "file_loader.h"
#include "primitive_mesh_data.h"
#include "resource_manager.h"

using namespace std;

static constexpr int MAX_PARTICLES = 10000;
GLsizei vec4_size = sizeof(glm::vec4);
GLsizei pid_size = sizeof(particle_instance_data);
GLsizei ui_size = sizeof(GLuint);
GLint last_used_particle = 0;

ParticleSystem::ParticleSystem(Material* particle_material, Material* box_material, const particle_info& info) : InstancedObject(particle_material)
{
    particles_payload_info = info;
    m_particle_amount = particles_payload_info.amount;
    m_particle_lifetime = particles_payload_info.lifetime;
    m_position = glm::vec3(0.0f);
    m_particle_rotation = particles_payload_info.rotation;
    m_particle_velocity = particles_payload_info.velocity;
    m_particle_color = particles_payload_info.color;
    m_particle_range = particles_payload_info.range;
    m_particle_scale = particles_payload_info.scale;
    
    initialize();
    set_up_particle_buffers();

    m_bounding_box = new BoundingBox(box_material);

    static_assert(sizeof(particle_instance_data) == 36, "ERROR::PARTICLESYSTEM::WRONG_STRUCT_SIZE");

    cout << "Particle System has been activated with " << m_particle_amount << " particles." << '\n';
}

ParticleSystem::~ParticleSystem()
{
    if (!m_particles.empty())
    {
        m_particles.clear();
    }
}

void ParticleSystem::initialize()
{
    m_current_particle_sprite = ResourceManager::get_texture(particles_payload_info.texture_selection_name);

    for (GLuint i = 0; i < m_particle_amount; ++i)
    {
        auto p = particle();
        p.lifetime = particles_payload_info.randomize_lifetime ? m_particle_lifetime + random_float(-particles_payload_info.lifetime_random_offset, particles_payload_info.lifetime_random_offset) : m_particle_lifetime;
        p.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), 1.0f) : m_particle_color;
        p.position = m_position + glm::vec3(random_float(-m_particle_range, m_particle_range), 0.0f, random_float(-m_particle_range, m_particle_range));
        p.rotation = m_particle_rotation;
        p.velocity = m_particle_velocity;
        p.scale = particles_payload_info.randomize_scale ? m_particle_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : m_particle_scale;
        m_particles.push_back(p);
    }
}

void ParticleSystem::set_up_particle_buffers()
{
    cout << "Setting up particle buffer data..." << '\n';

    m_vbo = new VertexBuffer(static_cast<GLsizei>(QUAD_VERTICES.size()) * vec4_size, QUAD_VERTICES.data());
    m_ebo = new ElementBuffer(static_cast<GLsizei>(QUAD_INDICES.size()) * ui_size, QUAD_INDICES.data());
    m_ibo = new InstanceBuffer(m_particle_amount * pid_size, nullptr, GL_DYNAMIC_DRAW);
    
    m_vao->set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, vec4_size, nullptr); // Vertex Attribute: vertexPosition/TextCoords (vec4) -> layout(location = 0)

    // Instance attributes
    // Attribute pointer parameters order: index, size, type, normalized, stride, pointer, divisor (0 by default)
    m_vao->set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, position)), 1); // Vertex Attribute: instancePosition (vec3)  -> layout(location = 1)
    m_vao->set_attrib_pointer(2, 1, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, rotation)), 1); // Vertex Attribute: instanceRotation (float) -> layout(location = 2)
    m_vao->set_attrib_pointer(3, 4, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, color)), 1);    // Vertex Attribute: instanceColor    (vec4)  -> layout(location = 3)
    m_vao->set_attrib_pointer(4, 1, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, scale)), 1);    // Vertex Attribute: instanceScale    (float) -> layout(location = 4)

    m_vao->unbind_object();

    // Set size of particle instance data payload
    m_particle_instance_data.resize(m_particle_amount);
}

void ParticleSystem::sort_particles() const
{
    auto cameraPos = m_camera->get_camera_position();
    ranges::sort(m_particles,
                 [cameraPos](const particle& a, const particle& b) {
                     float distA = length(glm::vec3(a.position) - cameraPos);
                     float distB = length(glm::vec3(b.position) - cameraPos);
                     return distA > distB; // Sort back to front
                 });
}

float ParticleSystem::random_float(float min, float max)
{
    static random_device rd;
    static mt19937 generator(rd());
    uniform_real_distribution distribution(min, max);
    return distribution(generator);
}

void ParticleSystem::update(float dt)
{
    if (!m_is_visible) return;

    // add new particles 
    for (GLuint i = 0; i < 2; ++i)
    {
        auto unused_particle = first_unused_particle();
        if (m_particles[unused_particle].lifetime <= 0.0f || m_particles[unused_particle].color.a <= 0.0f)
        {
            respawn_particle(m_particles[unused_particle]);
        }
    }

    m_particle_range = particles_payload_info.range;
    m_particle_rotation = particles_payload_info.rotation;
    m_particle_scale = particles_payload_info.scale;
    m_particle_lifetime = particles_payload_info.lifetime;

    if (!particles_payload_info.randomize_velocity)
    {
        m_particle_velocity = particles_payload_info.velocity;
    }

    // update all particles
    for (particle& p : m_particles)
    {
        if (!particles_payload_info.randomize_color)
        {
            p.color.r = particles_payload_info.color.r;
            p.color.g = particles_payload_info.color.g;
            p.color.b = particles_payload_info.color.b;
        }

        if (!particles_payload_info.randomize_rotation)
        {
            p.rotation = m_particle_rotation;
        }

        if (!particles_payload_info.randomize_scale)
        {
            p.scale = particles_payload_info.scale;
        }

        if (!particles_payload_info.randomize_velocity)
        {
            p.velocity = particles_payload_info.velocity;
        }

        p.lifetime -= dt; // reduce life

        if (p.lifetime > 0.0f)
        {
            p.position += p.velocity * dt; // particle is alive, thus update
            if (particles_payload_info.is_color_faded)
            {
                p.color.a -= (dt / p.lifetime);
            }
            else
            {
                p.color.a = particles_payload_info.color.a;
            }
        }
    }

    for (GLuint i = 0; i < m_particles.size(); ++i)
    {
        m_particle_instance_data[i].position = m_particles[i].position;
        m_particle_instance_data[i].rotation = m_particles[i].rotation;
        m_particle_instance_data[i].color = m_particles[i].color;
        m_particle_instance_data[i].scale = m_particles[i].scale;
    }

    sort_particles();

    m_ibo->bind_object();
    m_ibo->set_buffer_sub_data(0, m_particle_instance_data);
    m_ibo->unbind_object();

    Object::update(dt);
    m_bounding_box->update(dt);
}

void ParticleSystem::draw() const
{
    // use additive blending to give it a 'glow' effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_vao->bind_object();
    // Disable depth mask to prevent writing to the depth buffer
    glDepthMask(GL_FALSE);
    
    m_material->set_int("sprite", 0);
    InstancedObject::draw();
    
    if (const auto selected_texture = ResourceManager::get_texture(particles_payload_info.texture_selection_name); m_current_particle_sprite != selected_texture)
    {
        m_current_particle_sprite = selected_texture;
    }
    m_current_particle_sprite->bind(0);

    // Draw particles using instancing
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(QUAD_INDICES.size()), GL_UNSIGNED_INT, nullptr, m_particle_amount);
    m_vao->unbind_object();

    // don't forget to reset to default blending mode
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    if (particles_payload_info.render_bounding_box)
    {
        m_bounding_box->draw();
    }

    Texture2D::unbind();
}

GLuint ParticleSystem::first_unused_particle() const
{
    // first search from last used particle, this will usually return almost instantly
    for (GLint i = last_used_particle; i < m_particle_amount; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f || m_particles[i].color.a <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (GLint i = 0; i < last_used_particle; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f || m_particles[i].color.a <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    last_used_particle = 0;
    return last_used_particle;
}

void ParticleSystem::respawn_particle(particle& particle)
{
    particle.position = particles_payload_info.randomize_velocity ?
        glm::vec3(random_float(-m_particle_range, m_particle_range), random_float(-m_particle_range, m_particle_range), random_float(-m_particle_range, m_particle_range))
            :
        glm::vec3(random_float(-m_particle_range, m_particle_range), 0.0f, random_float(-m_particle_range, m_particle_range));

    particle.rotation = particles_payload_info.randomize_rotation ? random_float(0.0f, 360.0f) : m_particle_rotation;

    particle.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), particles_payload_info.color.a) : particles_payload_info.color;

    particle.lifetime = particles_payload_info.randomize_lifetime ? m_particle_lifetime + random_float(-particles_payload_info.lifetime_random_offset, 0.0f) : m_particle_lifetime;

    particle.velocity = particles_payload_info.randomize_velocity ?
        glm::vec3(random_float(-particles_payload_info.velocity_random_offset.x, particles_payload_info.velocity_random_offset.x),
                  random_float(-particles_payload_info.velocity_random_offset.y, particles_payload_info.velocity_random_offset.y),
                  random_float(-particles_payload_info.velocity_random_offset.z, particles_payload_info.velocity_random_offset.z))
        : particles_payload_info.velocity;

    particle.scale = particles_payload_info.randomize_scale ? m_particle_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : m_particle_scale;
}

void ParticleSystem::reset_particle_generator()
{
    particles_payload_info = particle_info();
}