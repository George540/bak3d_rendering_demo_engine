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

class Object
{
public:
    Object(Camera& camera);
    Object(Camera& camera, const string shader_name);
    virtual ~Object();

    virtual void initialize();
    virtual void draw() const;
    virtual void delete_globjects() const;
protected:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;

    Shader* m_shader;
    Camera* m_camera;
};

class IndexedObject : Object
{
public:
    IndexedObject(Camera& camera);
    IndexedObject(Camera& camera, const string shader_name);
    ~IndexedObject();

    virtual void initialize() override;
    void draw() const override;
    void delete_globjects() const override;
protected:
    ElementBuffer* m_ebo;
};

class InstancedObject : Object
{
public:
    InstancedObject(Camera& camera);
    InstancedObject(Camera& camera, const string shader_name);
    ~InstancedObject();

    virtual void initialize() override;
    void draw() const override;
    void delete_globjects() const override;
protected:
    InstanceBuffer* m_ibo;
};

class OptimizedObject : Object
{
public:
    OptimizedObject(Camera& camera);
    OptimizedObject(Camera& camera, const string shader_name);
    ~OptimizedObject();

    virtual void initialize() override;
    void draw() const override;
    void delete_globjects() const override;
protected:
    ElementBuffer* m_ebo;
    InstanceBuffer* m_ibo;
};

#endif