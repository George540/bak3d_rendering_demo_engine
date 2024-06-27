#include <iostream>
#include <random>
#include <glm/ext/matrix_transform.hpp>

#include "stb_image.h"
#include "particle_generator.h"
#include "file_loader.h"

using namespace std;

GLuint last_used_particle = 0;

ParticleGenerator::ParticleGenerator(Camera& camera, particle_info info) : m_camera(&camera)
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
}

ParticleGenerator::~ParticleGenerator()
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

void ParticleGenerator::initialize()
{
    // set up mesh and attribute properties
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
    glBindVertexArray(m_particle_VAO);

    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_particle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindVertexArray(0);

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

    std::cout << "Particle System has been activated with " << m_amount << " particles." << std::endl;

    // BOUNDING BOX DATA
    
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

void ParticleGenerator::sort_particles()
{
    auto cameraPos = m_camera->get_camera_position();
    std::sort(m_particles.begin(), m_particles.end(),
        [cameraPos](const particle& a, const particle& b) {
            float distA = glm::length(glm::vec3(a.position) - cameraPos);
            float distB = glm::length(glm::vec3(b.position) - cameraPos);
            return distA > distB; // Sort back to front
        });
}

float ParticleGenerator::random_float(float min, float max)
{
    static random_device rd;
    static mt19937 generator(rd());
    uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

void ParticleGenerator::update(float dt, GLuint new_particles)
{
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

void ParticleGenerator::draw()
{
    if (!m_particle_shader) return;

    // use additive blending to give it a 'glow' effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    sort_particles();
    m_particle_shader->use();

    // Disable depth mask to prevent writing to the depth buffer
    glDepthMask(GL_FALSE);

    for (const particle& p : m_particles)
    {
        if (p.lifetime > 0.0f)
        {
            m_particle_shader->set_vec3("position", p.position);
            m_particle_shader->set_float("rotation", p.rotation);
            m_particle_shader->set_float("scale", p.scale);
            m_particle_shader->set_vec4("color", p.color);
            m_particle_shader->set_mat4("projection", m_camera->get_projection_matrix());
            m_particle_shader->set_mat4("view", m_camera->get_view_matrix());

            auto selected_texture = m_textures_loaded[particles_payload_info.texture_selection];
            if (m_texture != selected_texture)
            {
                m_texture = selected_texture;
            }
            m_texture.bind();

            glBindVertexArray(m_particle_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    // Bounding Box
    if (!particles_payload_info.randomize_velocity && particles_payload_info.render_bounding_box)
    {
        glDepthFunc(GL_ALWAYS);

        m_bounding_box_shader->use();
        m_bounding_box_shader->set_mat4("projection", m_camera->get_projection_matrix());
        m_bounding_box_shader->set_mat4("view", m_camera->get_view_matrix());

        auto model = glm::mat4(1.0f);
        auto horizontal_scale = m_range * 2;
        model = glm::scale(model, glm::vec3(horizontal_scale, m_lifetime * m_velocity.y, horizontal_scale));
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
        m_bounding_box_shader->set_mat4("model", model);
        m_bounding_box_shader->set_vec3("color", glm::vec3(1.0f));

        glBindVertexArray(m_bb_VAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }
}

GLuint ParticleGenerator::first_unused_particle()
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

void ParticleGenerator::respawn_particle(particle& particle)
{
    particle.position = m_position + glm::vec3(random_float(-m_range, m_range), 0.0f, random_float(-m_range, m_range));

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

void ParticleGenerator::delete_vao_vbo() const
{
    glDeleteVertexArrays(1, &m_particle_VAO);
    glDeleteBuffers(1, &m_particle_VBO);
    glDeleteVertexArrays(1, &m_bb_VAO);
    glDeleteBuffers(1, &m_bb_VBO);
}