#include <iostream>
#include <random>
#include <glm/ext/matrix_transform.hpp>

#include "stb_image.h"
#include "particle_generator.h"
#include "file_loader.h"

using namespace std;

static const int MAX_PARTICLES = 10000;
GLsizei vec4_size = sizeof(glm::vec4);
GLsizei pid_size = sizeof(particle_instance_data);
GLuint last_used_particle = 0;

ParticleSystem::ParticleSystem(Camera& camera, particle_info info) : m_camera(&camera)
{
    m_particle_shader = new Shader(std::filesystem::absolute("shaders/ParticleShader.vs").string().c_str(),
                      std::filesystem::absolute("shaders/ParticleShader.fs").string().c_str());
    particles_payload_info = info;
    m_amount = particles_payload_info.amount;
    m_lifetime = particles_payload_info.lifetime;
    m_position = glm::vec3(0.0f);
    m_rotation = particles_payload_info.rotation;
    m_velocity = particles_payload_info.velocity;
    m_color = particles_payload_info.color;
    m_range = particles_payload_info.range;
    m_scale = particles_payload_info.scale;

    initialize();
    set_up_particle_buffers();
    initialize_bounding_box();

    cout << "Particle System has been activated with " << m_amount << " particles." << endl;
}

ParticleSystem::~ParticleSystem()
{
    if (m_particle_shader)
    {
        delete m_particle_shader;
    }
    m_camera = nullptr;

    if (!m_particles.empty())
    {
        m_particles.clear();
    }

    delete_vao_vbo();
}

void ParticleSystem::initialize()
{
    auto particle_texture_data = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/particles-textures"), FileType::png);
    for (const auto& pair : particle_texture_data)
    {
        const auto& path = pair.second;
        Texture2D t = Texture2D(path, aiTextureType_DIFFUSE);
        m_textures_loaded.push_back(t);
    }

    m_texture = m_textures_loaded[particles_payload_info.texture_selection];

    for (GLuint i = 0; i < m_amount; ++i)
    {
        auto p = particle();
        p.lifetime = particles_payload_info.randomize_lifetime ? m_lifetime + random_float(-particles_payload_info.lifetime_random_offset, particles_payload_info.lifetime_random_offset) : m_lifetime;
        p.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), 1.0f) : m_color;
        p.position = m_position + glm::vec3(random_float(-m_range, m_range), 0.0f, random_float(-m_range, m_range));
        p.rotation = m_rotation;
        p.velocity = m_velocity;
        p.scale = particles_payload_info.randomize_scale ? m_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : m_scale;
        m_particles.push_back(p);
    }
}

void ParticleSystem::set_up_particle_buffers()
{
    cout << "Setting up particle buffer data..." << endl;

    // set up particle quad vertex data
    float particle_quad[] =
    {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_particle_VAO);
    glGenBuffers(1, &m_particle_VBO);
    glGenBuffers(1, &m_instance_VBO);

    glBindVertexArray(m_particle_VAO);

    // Vertex buffer (particle quad)
    glBindBuffer(GL_ARRAY_BUFFER, m_particle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // Vertex attributes (vertex position, texture coordinates)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vec4_size, nullptr);
    glEnableVertexAttribArray(0);

    // Instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_amount * sizeof(particle_instance_data), nullptr, GL_DYNAMIC_DRAW);

    // Instance attributes
    // Attribute pointer parameters order: index, size, type, normalized, stride, pointer

    // Particle Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // advance once per instance

    // Particle Rotation
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, rotation)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Particle Color
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, color)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Particle Scale
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, pid_size, reinterpret_cast<void*>(offsetof(particle_instance_data, scale)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);

    // Set size of particle instance data payload
    m_particle_instance_data.resize(m_amount);
}

void ParticleSystem::initialize_bounding_box()
{
    cout << "Initializing Particle System Bounding Box data..." << endl;

    m_bounding_box_shader = new Shader(std::filesystem::absolute("shaders/GridShader.vs").string().c_str(),
        std::filesystem::absolute("shaders/GridShader.fs").string().c_str());

    // Define the 8 vertices of the cube
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  // Vertex 0
         0.5f, -0.5f, -0.5f,  // Vertex 1
         0.5f,  0.5f, -0.5f,  // Vertex 2
        -0.5f,  0.5f, -0.5f,  // Vertex 3
        -0.5f, -0.5f,  0.5f,  // Vertex 4
         0.5f, -0.5f,  0.5f,  // Vertex 5
         0.5f,  0.5f,  0.5f,  // Vertex 6
        -0.5f,  0.5f,  0.5f   // Vertex 7
    };

    // Define the edges of the cube (pairs of vertices)
    GLuint indices[] = {
        0, 1,  1, 2,  2, 3,  3, 0,  // Bottom face
        4, 5,  5, 6,  6, 7,  7, 4,  // Top face
        0, 4,  1, 5,  2, 6,  3, 7   // Side edges
    };

    // Create and bind the VAO
    glGenVertexArrays(1, &m_bb_VAO);
    glBindVertexArray(m_bb_VAO);

    // Create and bind the VBO
    glGenBuffers(1, &m_bb_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_bb_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and bind the EBO
    glGenBuffers(1, &m_bb_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bb_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define the vertex attribute data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VAO
    glBindVertexArray(0);
}

void ParticleSystem::sort_particles()
{
    auto cameraPos = m_camera->get_camera_position();
    std::sort(m_particles.begin(), m_particles.end(),
        [cameraPos](const particle& a, const particle& b) {
            float distA = glm::length(glm::vec3(a.position) - cameraPos);
            float distB = glm::length(glm::vec3(b.position) - cameraPos);
            return distA > distB; // Sort back to front
        });
}

float ParticleSystem::random_float(float min, float max)
{
    static random_device rd;
    static mt19937 generator(rd());
    uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

void ParticleSystem::update(float dt, GLuint new_particles)
{
    // Update instance buffer 
    for (GLuint i = 0; i < m_amount; ++i)
    {
        m_particle_instance_data[i].position = m_particles[i].position;
        m_particle_instance_data[i].rotation = m_particles[i].rotation;
        m_particle_instance_data[i].color = m_particles[i].color;
        m_particle_instance_data[i].scale = m_particles[i].scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(particle_instance_data), m_particle_instance_data.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // add new particles 
    for (GLuint i = 0; i < new_particles; ++i)
    {
        auto unused_particle = first_unused_particle();
        if (m_particles[unused_particle].lifetime <= 0.0f || m_particles[unused_particle].color.a <= 0.0f)
        {
            respawn_particle(m_particles[unused_particle]);
        }
    }

    m_range = particles_payload_info.range;
    m_rotation = particles_payload_info.rotation;
    m_scale = particles_payload_info.scale;
    m_lifetime = particles_payload_info.lifetime;

    if (!particles_payload_info.randomize_velocity)
    {
        m_velocity = particles_payload_info.velocity;
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
            p.rotation = m_rotation;
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
}

void ParticleSystem::draw()
{
    if (!m_particle_shader) return;

    // use additive blending to give it a 'glow' effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    sort_particles();
    m_particle_shader->use();
    // Update instance buffer
    glBindVertexArray(m_particle_VAO);

    // Disable depth mask to prevent writing to the depth buffer
    glDepthMask(GL_FALSE);

    m_particle_shader->set_mat4("projection", m_camera->get_projection_matrix());
    m_particle_shader->set_mat4("view", m_camera->get_view_matrix());

    auto selected_texture = m_textures_loaded[particles_payload_info.texture_selection];
    if (m_texture != selected_texture)
    {
        m_texture = selected_texture;
    }
    m_texture.bind();
    // Draw particles using instancing
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_amount);
    glBindVertexArray(0);

    // don't forget to reset to default blending mode
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    if (particles_payload_info.render_bounding_box)
    {
        draw_bounding_box();
    }
}

void ParticleSystem::draw_bounding_box()
{
    glDepthFunc(GL_ALWAYS);

    m_bounding_box_shader->use();
    m_bounding_box_shader->set_mat4("projection", m_camera->get_projection_matrix());
    m_bounding_box_shader->set_mat4("view", m_camera->get_view_matrix());

    auto model = glm::mat4(1.0f);
    if (particles_payload_info.randomize_velocity)
    {
        auto velocity_offset = particles_payload_info.velocity_random_offset;
        model = glm::scale(model, (glm::vec3(velocity_offset) * m_lifetime) + (m_range * 2.0f));
    }
    else
    {
        auto horizontal_scale = m_range * 2;
        model = glm::scale(model, (glm::vec3(m_velocity) * m_lifetime) + glm::vec3(horizontal_scale, 0.0f, horizontal_scale));
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    }

    m_bounding_box_shader->set_mat4("model", model);
    m_bounding_box_shader->set_vec3("color", glm::vec3(1.0f));

    glBindVertexArray(m_bb_VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

GLuint ParticleSystem::first_unused_particle()
{
    // first search from last used particle, this will usually return almost instantly
    for (GLuint i = last_used_particle; i < m_amount; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f || m_particles[i].color.a <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (GLuint i = 0; i < last_used_particle; ++i)
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
        glm::vec3(random_float(-m_range, m_range), random_float(-m_range, m_range), random_float(-m_range, m_range))
            :
        glm::vec3(random_float(-m_range, m_range), 0.0f, random_float(-m_range, m_range));

    particle.rotation = particles_payload_info.randomize_rotation ? random_float(0.0f, 360.0f) : m_rotation;

    particle.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), particles_payload_info.color.a) : particles_payload_info.color;

    particle.lifetime = particles_payload_info.randomize_lifetime ? m_lifetime + random_float(-particles_payload_info.lifetime_random_offset, 0.0f) : m_lifetime;

    particle.velocity = particles_payload_info.randomize_velocity ?
        glm::vec3(random_float(-particles_payload_info.velocity_random_offset.x, particles_payload_info.velocity_random_offset.x),
                  random_float(-particles_payload_info.velocity_random_offset.y, particles_payload_info.velocity_random_offset.y),
                  random_float(-particles_payload_info.velocity_random_offset.z, particles_payload_info.velocity_random_offset.z))
        : particles_payload_info.velocity;

    particle.scale = particles_payload_info.randomize_scale ? m_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : m_scale;
}

void ParticleSystem::delete_vao_vbo() const
{
    glDeleteVertexArrays(1, &m_particle_VAO);
    glDeleteBuffers(1, &m_particle_VBO);
    glDeleteBuffers(1, &m_instance_VBO);
    glDeleteVertexArrays(1, &m_bb_VAO);
    glDeleteBuffers(1, &m_bb_VBO);
}