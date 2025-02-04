// =====================================================================================
// Object class was made by George Mavroeidis as a base class for engine objects.
// It contains a set of virtual functions and data that help with initializing, binding
// drawing and cleaning up the object in the scene.
// =====================================================================================

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>

#include "camera.h"
#include "shader.h"
#include "buffer.h"
#include "vertex_array.h"

struct GLObjectData
{
    VertexArray m_vao;
    VertexBuffer m_vbo;
    ElementBuffer m_ebo;
    InstanceBuffer m_ibo;
};

class Object
{
public:
    Object(Camera& camera);
    ~Object();

    void draw() const;
    void delete_globjects() const;
protected:
    GLObjectData* m_globject_data;
    Shader* m_shader;
    Camera* m_camera;
};

#endif