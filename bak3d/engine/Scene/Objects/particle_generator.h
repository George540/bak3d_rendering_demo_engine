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

/*#pragma once

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>

#include "bounding_box.h"
#include "renderable_object.h"
#include "Renderer/material.h"

struct particle
{
	glm::vec3 position;
    float rotation;
	glm::vec3 velocity;
	glm::vec4 color;
	float lifetime;
    float scale;
    float max_lifetime;

    particle() : 
        position(0.0f),
        rotation(0.0f),
        velocity(0.0f),
        color(1.0f),
        lifetime(0.0f),
        scale(1.0f),
        max_lifetime(1.0f)
    {}
};

struct particle_instance_data
{
    glm::vec3 position;
    float rotation;
    glm::vec4 color;
    float scale;

    particle_instance_data() :
        position(0.0f),
        rotation(0.0f),
        color(1.0f),
        scale(1.0f)
    {}
};

struct particle_info
{
    int amount;
    float rotation;
    glm::vec3 velocity;
    glm::vec4 color;
    float lifetime;
    float scale;
    float range;
    int texture_selection_index;
    std::string texture_selection_name;

    bool randomize_rotation;
    bool randomize_lifetime;
    bool randomize_velocity;
    bool render_bounding_box;
    glm::vec3 velocity_random_offset;
    float lifetime_random_offset;
    bool randomize_color;
    bool is_color_faded;
    bool randomize_scale;
    float scale_random_offset;

    particle_info() :
        amount(50),
        rotation(0.0f),
        velocity(0.0f, 1.0f, 0.0f),
        color(1.0f),
        lifetime(3.0f),
        scale(0.5f),
        range(1.0f),
        texture_selection_index(2),
        randomize_rotation(false),
        randomize_lifetime(false),
        randomize_velocity(false),
        render_bounding_box(false),
        velocity_random_offset(1.0f, 1.0f, 1.0f),
        lifetime_random_offset(0.0f),
        randomize_color(false),
        is_color_faded(false),
        randomize_scale(false),
        scale_random_offset(0.1f)
        {}
};

/*
 * @todo TO BE DEPRECATED AND REMADE:
 * Particle System class that is based on instanced object properties to generate billboard particles
 * and manage them in 3D space with dynamic lifetime and other properties that change over time.
 #1#
class ParticleSystem : public InstancedObject
{
public:
    // constructor
    ParticleSystem(Material* particle_material, Material* box_material, const particle_info& info = particle_info());
    ~ParticleSystem();

    void update(float dt) override;
    void draw() const override;

    void set_visible(bool visible) { m_is_visible = visible; }
    bool is_visible() const { return m_is_visible; }

    void set_particle_info_from_payload(const particle_info& info);
    void sort_particles() const;
    static float random_float(float min, float max);
    GLuint get_particle_amount() const { return m_particle_amount; }
    void update(float dt, GLuint new_particles);
private:
    mutable std::vector<particle> m_particles;
    std::vector<particle_instance_data> m_particle_instance_data;
    bool m_is_visible;

    particle_info particles_payload_info;
    GLint m_particle_amount;
    float m_particle_rotation;
    glm::vec3 m_particle_velocity;
    glm::vec4 m_particle_color;
    float m_particle_lifetime;
    float m_particle_range;
    float m_particle_scale;

    GLint m_last_used_particle = 0;
    float m_spawn_accumulator = 0.0f;
    float m_emission_rate = 2.0f; // particles per second

    mutable Texture2D* m_current_particle_sprite;
    BoundingBox* m_bounding_box;
    
    void initialize(); // initializes particle data
    void set_up_particle_buffers(); // initializes particle buffers
    GLuint first_unused_particle(); // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    void respawn_particle(particle& particle); // respawns particle
    particle make_particle();
    void reset_particle_generator();
};

#endif*/