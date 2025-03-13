// =====================================================================================
// Transform class was made by George Mavroeidis as a base class for entity orientation
// in space based on position, orientation and scaling. It contains a bunch of functions
// and data for updating the object in space, most importantly, using a model matrix.
// =====================================================================================

#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
protected:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_euler_rotation = glm::vec3(0.0f, 1.0f, 0.0f); // up vector
    glm::vec3 m_scaling = glm::vec3(1.0f);

    glm::mat4 m_model_matrix = glm::mat4(1.0f);

public:
    Transform() {};
    ~Transform() {};

    glm::vec3 get_position() const { return m_position; }
    glm::vec3 get_rotation() const { return m_euler_rotation; }
    glm::vec3 get_scaling() const { return m_scaling; }

    void set_position(const glm::vec3& position) { m_position = position; }
    void set_rotation(const glm::vec3& rotation) { m_euler_rotation = rotation; }
    void set_scaling(const glm::vec3& scaling) { m_scaling = scaling; }

    glm::mat4 get_model_matrix() const { return m_model_matrix; }
    void set_model_matrix(const glm::vec3& position, const glm::vec3& scaling, const glm::vec3& rotation_axis, const float rotation_angle_degrees)
    { 
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix =
            translate(m_model_matrix, m_position) *
            rotate(m_model_matrix, glm::radians(rotation_angle_degrees), rotation_axis) *
            scale(m_model_matrix, m_scaling);
    }

    virtual void update(float dt) = 0;
};

#endif
