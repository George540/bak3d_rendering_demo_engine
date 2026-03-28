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

#include "bounding_box.h"

#include "user_interface.h"
#include "Renderer/mesh_data.h"

BoundingBox::BoundingBox(Material* material) : Object(material)
{
    m_vao->bind_object();
    m_vbo = new VertexBuffer(sizeof(glm::vec3) * CUBE_VERTICES_WIREFRAME.size(), CUBE_VERTICES_WIREFRAME.data());
    m_ebo = new ElementBuffer(sizeof(GLuint) * CUBE_INDICES_WIREFRAME.size(), CUBE_INDICES_WIREFRAME.data());

    m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    m_vao->unbind_object();
}

void BoundingBox::update(float dt)
{
    auto particles_info = UserInterface::particle_payload_info;
    if (particles_info.randomize_velocity)
    {
        auto velocity_offset = particles_info.velocity_random_offset;
        m_scaling = (glm::vec3(velocity_offset) * particles_info.lifetime) + (particles_info.range * 2.0f);
        Object::update(dt);
    }
    else
    {
        auto horizontal_scale = particles_info.range * 2;
        m_position = glm::vec3(0.0f, 0.5f, 0.0f);
        m_scaling = (glm::vec3(particles_info.velocity) * particles_info.lifetime) + glm::vec3(horizontal_scale, 0.0f, horizontal_scale);
    }

    Object::update(dt);
}

void BoundingBox::draw() const
{
    glDepthFunc(GL_ALWAYS);

    m_material->set_vec3("color", glm::vec3(1.0f));
    
    Object::draw();

    m_vao->bind_object();
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();

    glDepthFunc(GL_LESS);
}

