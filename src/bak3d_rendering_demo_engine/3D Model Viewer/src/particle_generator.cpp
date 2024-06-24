#include <iostream>
#include <random>

#include "stb_image.h"
#include "particle_generator.h"
#include <file_loader.h>

using namespace std;

GLuint last_used_particle = 0;

ParticleGenerator::ParticleGenerator(Camera& camera, particle_info info) : m_camera(&camera)
{
    m_shader = new Shader(std::filesystem::absolute("shaders/ParticleShader.vs").string().c_str(),
                      std::filesystem::absolute("shaders/ParticleShader.fs").string().c_str());
    particles_payload_info = info;
    m_amount = particles_payload_info.amount;
    m_lifetime = particles_payload_info.lifetime;
    m_position = glm::vec3(0.0f);
    m_velocity = particles_payload_info.velocity;
    m_color = particles_payload_info.color;
    m_range = particles_payload_info.range;
    m_scale = particles_payload_info.scale;
    initialize();
}

ParticleGenerator::~ParticleGenerator()
{
    if (m_shader)
    {
        delete m_shader;
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

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);

    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindVertexArray(0);

    auto particle_texture_data = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/particles-textures"), FileType::png);
    for (const auto& pair : particle_texture_data)
    {
        texture t;
        t.id = texture_from_file(pair.second);
        t.path = pair.second;
        t.type = aiTextureType_DIFFUSE;
        m_textures_loaded.push_back(t);
    }

    auto texture_info = m_textures_loaded[particles_payload_info.texture_selection];
    m_texture.path = texture_info.path;
    m_texture.id = texture_info.id;
    m_texture.type = texture_info.type;

    for (GLuint i = 0; i < m_amount; ++i)
    {
        auto p = particle();
        p.lifetime = particles_payload_info.randomize_lifetime ? m_lifetime + random_float(-particles_payload_info.lifetime_random_offset, particles_payload_info.lifetime_random_offset) : m_lifetime;
        p.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), 1.0f) : m_color;
        p.position = m_position + glm::vec3(random_float(-m_range, m_range), random_float(0.0f, m_range), random_float(-m_range, m_range));
        p.velocity = m_velocity;
        p.scale = particles_payload_info.randomize_scale ? m_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : m_scale;
        m_particles.push_back(p);
    }

    std::cout << "Particle System has been activated with " << m_amount << " particles." << std::endl;
}

// TODO: Create Texture2D class with appropriate loading functions
GLuint ParticleGenerator::texture_from_file(const string& path)
{
    auto filename = string(path);
    std::cout << "Loading texture file: " << filename << std::endl;

    GLuint texture_color_buffer;
    glGenTextures(1, &texture_color_buffer);

    int width, height, nr_components;
    const auto data = stbi_load(filename.c_str(), &width, &height, &nr_components, 0);

    if (data)
    {
        GLenum format;
        switch (nr_components)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGB;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);  // NOLINT(bugprone-narrowing-conversions)
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }
    else
    {
        cout << "ERROR::PARTICLEGENERATOR::FILE_NOT_SUCCESSFULLY_READ: Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return texture_color_buffer;
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
        if (m_particles[unused_particle].lifetime <= 0.0f)
        {
            respawn_particle(m_particles[unused_particle]);
        }
    }

    m_range = particles_payload_info.range;
    // update all particles
    for (particle& p : m_particles)
    {
        if (!particles_payload_info.randomize_color)
        {
            p.color.r = particles_payload_info.color.r;
            p.color.g = particles_payload_info.color.g;
            p.color.b = particles_payload_info.color.b;
        }

        if (!particles_payload_info.randomize_scale)
        {
            p.scale = particles_payload_info.scale;
        }

        p.velocity = particles_payload_info.velocity;

        p.lifetime -= dt; // reduce life

        if (p.lifetime > 0.0f)
        {	
            p.position += p.velocity * dt; // particle is alive, thus update
            if (particles_payload_info.is_color_faded)
            {
                p.color.a -= (dt / p.lifetime);
            }
        }
    }
}

void ParticleGenerator::draw()
{
    if (!m_shader) return;

    // use additive blending to give it a 'glow' effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    sort_particles();
    m_shader->use();

    // Disable depth mask to prevent writing to the depth buffer
    glDepthMask(GL_FALSE);

    for (const particle& p : m_particles)
    {
        if (p.lifetime > 0.0f)
        {
            m_shader->set_vec3("position", p.position);
            m_shader->set_float("scale", p.scale);
            m_shader->set_vec4("color", p.color);
            m_shader->set_mat4("projection", m_camera->get_projection_matrix());
            m_shader->set_mat4("view", m_camera->get_view_matrix());
            glBindTexture(GL_TEXTURE_2D, m_textures_loaded[particles_payload_info.texture_selection].id);
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}

GLuint ParticleGenerator::first_unused_particle()
{
    // first search from last used particle, this will usually return almost instantly
    for (GLuint i = last_used_particle; i < m_amount; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (GLuint i = 0; i < last_used_particle; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f)
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
    particle.position = m_position + glm::vec3(random_float(-m_range, m_range), random_float(0, m_range), random_float(-m_range, m_range));
    particle.color = particles_payload_info.randomize_color ? glm::vec4(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), 1.0f) : m_color;
    particle.lifetime = particles_payload_info.randomize_lifetime ? m_lifetime + random_float(-particles_payload_info.lifetime_random_offset, particles_payload_info.lifetime_random_offset) : m_lifetime;
    particle.velocity = particles_payload_info.velocity;
    particle.scale = particles_payload_info.randomize_scale ? m_scale - random_float(0.0f, particles_payload_info.scale_random_offset) : particles_payload_info.scale;
}

void ParticleGenerator::delete_vao_vbo() const
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}