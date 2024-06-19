#include "particle_generator.h"

GLuint last_used_particle = 0;

ParticleGenerator::ParticleGenerator(Shader shader, texture texture, GLuint amount)
    : m_shader(shader), m_texture(texture), m_amount(amount)
{
    initialize();
}

void ParticleGenerator::initialize()
{
    // set up mesh and attribute properties
    GLuint VBO;

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
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_VAO);

    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    for (GLuint i = 0; i < m_amount; ++i)
    {
        m_particles.push_back(particle());
    }
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
    // use additive blending to give it a 'glow' effect
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_shader.use();
    for (const particle& p : m_particles)
    {
        if (p.lifetime > 0.0f)
        {
            m_shader.set_vec2("offset", p.position);
            m_shader.set_vec4("color", p.color);
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
    auto random = ((rand() % 100) - 50) / 10.0f;
    //auto rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.position = m_position + random + offset;
    particle.color = glm::vec4(1.0f);
    particle.lifetime = 1.0f;
    particle.velocity = m_velocity * 0.1f;
}