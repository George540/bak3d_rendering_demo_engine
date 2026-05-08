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

#pragma once

#include <vector>

#include "particle_definitions.h"
#include "Asset/asset_definitions.h"

/*
 * Stores all particle data in a form of Structure of Arrays
 */
class ParticleEmitter
{
public:
    explicit ParticleEmitter(const std::string& name, ParticleEmitterConfig config = {});
    ~ParticleEmitter();

    // Non-copyable, movable
    ParticleEmitter(const ParticleEmitter&) = delete;
    ParticleEmitter& operator=(const ParticleEmitter&) = delete;
    ParticleEmitter(ParticleEmitter&&) = default;

    void update(float dt, const glm::vec3& emitter_world_pos);

    // Caller uploads to GPU
    const std::vector<ParticleInstanceData>& get_instance_data() const { return m_instance_data; }
    int get_max_particles() const { return m_config.max_particles; }
    int get_alive_count() const { return m_alive_count; }

    void apply_config(const ParticleEmitterConfig& cfg);
    const ParticleEmitterConfig& get_config() const { return m_config; }
    ParticleEmitterConfig& get_config() { return m_config; }

    const std::string& get_name() const { return m_name; }

    TextureRef get_texture() const { return m_texture; }
    void set_texture(const TextureRef& tex) { m_texture = tex; }

    bool is_enabled() const { return m_enabled; }
    void set_enabled(bool enabled) { m_enabled = enabled; }

private:
    // Structure of Arrays particle pool
    // Positions
    std::vector<float> m_position_x;
    std::vector<float> m_position_y;
    std::vector<float> m_position_z;
    // Velocities
    std::vector<float> m_velocity_x;
    std::vector<float> m_velocity_y;
    std::vector<float> m_velocity_z;
    // Colors
    std::vector<float> m_color_r;
    std::vector<float> m_color_g;
    std::vector<float> m_color_b;
    std::vector<float> m_color_a;
    // Lifetime
    std::vector<float> m_current_lifetime; // dead when lesser or equal to 0.0f
    std::vector<float> m_max_lifetime; // original lifetime for fade ratio
    // Scale & rotation
    std::vector<float> m_scale;
    std::vector<float> m_rotation;

    // Recycling
    // Stack of dead particle indices available for reuse
    std::vector<int> m_dead_indices; // treated as a stack (back = top)
    int m_alive_count = 0;

    // GPU upload buffer
    std::vector<ParticleInstanceData> m_instance_data;

    // Internal helpers
    void resize_pool(int new_max);
    void spawn_one_particle(const glm::vec3& emitter_world_pos);
    void kill_one_particle(int index);

    // State
    std::string m_name;
    ParticleEmitterConfig m_config;
    TextureRef m_texture;
    float m_spawn_accumulator = 0.0f;
    bool m_enabled = true;

    int m_prev_max_particles = 0; // track config changes
};
