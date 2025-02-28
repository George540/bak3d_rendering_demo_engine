// =====================================================================================
// Object class was made by George Mavroeidis as a base class for engine objects.
// It contains a set of virtual functions and data that help with initializing, binding
// drawing and cleaning up the object in the scene.
// =====================================================================================

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>

#include "transform.h"
#include "camera.h"
#include "shader.h"
#include "buffer.h"
#include "vertex_array.h"

class Object : public Transform
{
protected:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    ElementBuffer* m_ebo;

    Shader* m_shader;
    Camera* m_camera;
public:
    Object(Camera& camera, Shader& shader);
    virtual ~Object();

    virtual void update(float dt) override;
    virtual void draw() const;
    void delete_globjects() const;
};

#endif