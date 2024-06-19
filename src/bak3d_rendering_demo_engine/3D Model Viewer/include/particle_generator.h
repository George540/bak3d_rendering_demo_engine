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

    particle() : position(0.0f), velocity(0.0f), color(1.0f), lifetime(0.0f) {}
};

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(std::string text_path, GLuint amount, Camera& camera);
    ~ParticleGenerator();
    void update(float dt, GLuint new_particles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    void draw();
    void delete_vao_vbo() const;
private:
    // particle state
    std::vector<particle> m_particles;
    GLuint m_amount;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_lifetime;

    // render state
    Shader* m_shader;
    Camera* m_camera;
    texture m_texture;
    GLuint m_VAO;
    GLuint m_VBO;
    
    void initialize(); // initializes buffer and vertex attributes
    GLuint texture_from_file(const string& path); // load texture from file (code based from model class)
    GLuint first_unused_particle(); // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    void respawn_particle(particle& particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f)); // respawns particle
};

#endif