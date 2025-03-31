#include "bounding_box.h"

#include "primitive_mesh_data.h"
#include "user_interface.h"

BoundingBox::BoundingBox(Material* material) : Object(material)
{
    m_vbo = new VertexBuffer(sizeof(glm::vec3) * CUBE_VERTICES_SOLID.size(), CUBE_VERTICES_SOLID.data());
    m_ebo = new ElementBuffer(sizeof(GLuint) * CUBE_INDICES_SOLID.size(), CUBE_INDICES_SOLID.data());

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

