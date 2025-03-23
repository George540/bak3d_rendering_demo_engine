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
    virtual ~Object();

    void set_camera(Camera& camera) { m_camera = &camera; }
    void set_material(Material* material) { m_material = material; }
    void update(float dt) override;
    virtual void draw() const;
    void delete_globjects() const;
};

#endif