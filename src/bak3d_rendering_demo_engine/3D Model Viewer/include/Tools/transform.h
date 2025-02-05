// =====================================================================================
// Transform class was made by George Mavroeidis as a base class for entity orientation
// in space based on position, orientation and scaling. It contains a bunch of functions
// and data for updating the object in space, most importantly, using a model matrix.
// =====================================================================================

#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
protected:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_euler_rotation = glm::vec3(0.0f); //In degrees
    glm::vec3 m_scaling = glm::vec3(1.0f);

    glm::mat4 m_model_matrix = glm::mat4(1.0f);

public:
    Transform() {};
    ~Transform() {};

    inline glm::vec3 get_position() const { return m_position; }
    inline glm::vec3 get_rotation() const { return m_euler_rotation; }
    inline glm::vec3 get_scaling() const { return m_scaling; }

    inline void set_position(const glm::vec3& position) { m_position = position; }
    inline void set_rotation(const glm::vec3& rotation) { m_euler_rotation = rotation; }
    inline void set_scaling(const glm::vec3& scaling) { m_scaling = scaling; }

    inline glm::mat4 get_model_matrix() const { return m_model_matrix; }
    inline void set_model_matrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scaling)
    { 
        m_model_matrix = glm::translate(m_model_matrix, m_position) * glm::rotate(m_model_matrix, 0.0f, rotation) * glm::scale(m_model_matrix, m_scaling);
    };

    virtual void update(float dt) = 0;
};

#endif
