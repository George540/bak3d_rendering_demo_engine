// =====================================================================================
// Buffer class was made by George Mavroeidis as a base class for opengl buffer objects.
// It contains implementations of GLObject class as well as more specific buffer
// generation functions and bindings. It can be used for any buffer type in OpenGL
// Buffer subclasses:
// - VertexBuffer
// - ElementBuffer
// - InstanceBuffer
// =====================================================================================

#pragma once

#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>

#include "globject.h"

class Buffer: GLObject
{
public:
    Buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    ~Buffer();
    void set_data(const void* data) { m_data = data; }
    void bind_object() const;
    void buffer_subdata();
    void unbind_object() const;

protected:
    GLenum m_target;
    GLsizeiptr m_size;
    const void* m_data;
    GLenum m_usage;
};

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ARRAY_BUFFER, size, data, usage) {}
};

class ElementBuffer : public Buffer
{
public:
    ElementBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ELEMENT_ARRAY_BUFFER, size, data, usage) {}
};

class InstanceBuffer : public Buffer
{
public:
    InstanceBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ARRAY_BUFFER, size, data, usage) {}
};

#endif