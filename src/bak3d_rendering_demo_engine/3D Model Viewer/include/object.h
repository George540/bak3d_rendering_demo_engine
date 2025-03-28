// =====================================================================================
// Object class was made by George Mavroeidis as a base class for engine objects.
// It contains a set of virtual functions and data that help with initializing, binding,
// drawing and cleaning up the object in the scene.
// =====================================================================================

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "transform.h"
#include "camera.h"
#include "material.h"
#include "buffer.h"
#include "vertex_array.h"

class Object : public Transform
{
protected:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    ElementBuffer* m_ebo;

    Material* m_material;
    Camera* m_camera;
public:
    Object(Material* material);
    ~Object() override;

    void set_camera(Camera& camera) { m_camera = &camera; }
    void set_material(Material* material) { m_material = material; }
    void update(float dt) override;
    virtual void draw() const;
};

class InstancedObject : public Object
{
protected:
    InstanceBuffer* m_ibo;
public:
    InstancedObject(Material* material);
    ~InstancedObject() override;
};

#endif