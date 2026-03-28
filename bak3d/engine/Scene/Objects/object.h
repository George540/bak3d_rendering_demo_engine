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

#ifndef OBJECT_H
#define OBJECT_H

#include "camera.h"
#include "Renderer/buffer.h"
#include "Renderer/material.h"
#include "Renderer/vertex_array.h"
#include "transform.h"

/*
 * Base primitive object class for drawable engine scene objects.
 * Contains data for initializing, binding, drawing and cleaning up.
 */
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

    virtual void set_camera(Camera& camera) { m_camera = &camera; }
    void set_material(Material* material) { m_material = material; }
    void update(float dt) override;
    virtual void draw() const;
};

/*
 * Instanced version of the object class, containing an instanced buffer.
 * Used for drawing multiple instances of the same object in the same draw call.
 */
class InstancedObject : public Object
{
protected:
    InstanceBuffer* m_ibo;
public:
    InstancedObject(Material* material);
    ~InstancedObject() override;

    void draw() const override;
};

#endif