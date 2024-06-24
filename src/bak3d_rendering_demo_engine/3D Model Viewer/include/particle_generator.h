// =====================================================================================
// This particle generator class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <shader.h>
#include <vector>
#include <mesh.h>
#include <camera.h>

struct particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float lifetime;
    float scale;

    particle() : position(0.0f), velocity(0.0f), color(1.0f), lifetime(0.0f), scale(0.0f) {}
};

struct particle_info
{
    int amount;
    glm::vec3 velocity;
    glm::vec4 color;
    float lifetime;
    float scale;
    float range;

    bool randomize_lifetime;
    float lifetime_random_offset;
    bool randomize_color;
    bool is_color_faded;
    bool randomize_scale;
    float scale_random_offset;

    particle_info() :
        amount(50),
        velocity(0.0f, 1.0f, 0.0f),
        color(1.0f),
        lifetime(3.0f),
        scale(0.5f),
        range(1.0f),
        randomize_lifetime(false),
        lifetime_random_offset(0.0f),
        randomize_color(false),
        is_color_faded(false),
        randomize_scale(false),
        scale_random_offset(0.1f)
        {}
};

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(std::string text_path, Camera& camera, particle_info info = particle_info());
    ~ParticleGenerator();
    void sort_particles();
    float random_float(float min, float max);
    const GLuint get_particle_amount() const { return m_amount; }
    void update(float dt, GLuint new_particles);
    void draw();
    void delete_vao_vbo() const;

    particle_info particles_payload_info;
private:
    // particle state
    std::vector<particle> m_particles;

    GLuint m_amount;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec4 m_color;
    float m_lifetime;
    float m_range;
    float m_scale;

    // render state
    Shader* m_shader;
    Camera* m_camera;
    texture m_texture;
    GLuint m_VAO;
    GLuint m_VBO;
    
    void initialize(); // initializes buffer and vertex attributes
    GLuint texture_from_file(const string& path); // load texture from file (code based from model class)
    GLuint first_unused_particle(); // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    void respawn_particle(particle& particle); // respawns particle
};

#endif