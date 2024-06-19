#include <iostream>
#include <filesystem>

#include "stb_image.h"
#include "particle_generator.h"

using namespace std;

GLuint last_used_particle = 0;

ParticleGenerator::ParticleGenerator(std::string text_path, GLuint amount, Camera& camera) : m_amount(amount), m_camera(&camera)
{
    m_shader = new Shader(std::filesystem::absolute("shaders/ParticleShader.vs").string().c_str(),
                      std::filesystem::absolute("shaders/ParticleShader.fs").string().c_str());
    m_texture.path = text_path;
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f, -0.1f, 0.0f);
    m_lifetime = 5.0f;
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

    m_texture.id = texture_from_file(m_texture.path);
    m_texture.type = aiTextureType_DIFFUSE;


    for (GLuint i = 0; i < m_amount; ++i)
    {
        auto p = particle();
        p.lifetime = m_lifetime;
        p.color = glm::vec4(1.0f);
        p.position = m_position;
        p.velocity = m_velocity;
        m_particles.push_back(p);
    }

    std::cout << "Particle System has been activated." << std::endl;
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

void ParticleGenerator::update(float dt, GLuint new_particles, glm::vec3 offset)
{
    // add new particles 
    for (GLuint i = 0; i < new_particles; ++i)
    {
        auto unused_particle = first_unused_particle();
        respawn_particle(m_particles[unused_particle], offset);
    }

    // update all particles
    for (GLuint i = 0; i < m_amount; ++i)
    {
        auto& particle = m_particles[i];
        particle.lifetime -= dt; // reduce life

        if (particle.lifetime > 0.0f)
        {	
            particle.position -= particle.velocity * dt; // particle is alive, thus update
            //p.color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::draw()
{
    if (!m_shader) return;

    sort_particles();

    // use additive blending to give it a 'glow' effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_shader->use();

    for (const particle& p : m_particles)
    {
        if (p.lifetime > 0.0f)
        {
            m_shader->set_vec2("offset", p.position);
            m_shader->set_vec4("color", p.color);
            m_shader->set_mat4("projection", m_camera->get_projection_matrix());
            m_shader->set_mat4("view", m_camera->get_view_matrix());
            glBindTexture(GL_TEXTURE_2D, m_texture.id);
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    return 0;
}

void ParticleGenerator::respawn_particle(particle& particle, glm::vec3 offset)
{
    float random = static_cast<float>((rand() % 10) - 5);
    //auto rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.position = m_position + glm::vec3(random, 0.0f, random) + offset;
    particle.color = glm::vec4(1.0f);
    particle.lifetime = m_lifetime;
    particle.velocity = m_velocity * 0.1f;
}

void ParticleGenerator::delete_vao_vbo() const
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}