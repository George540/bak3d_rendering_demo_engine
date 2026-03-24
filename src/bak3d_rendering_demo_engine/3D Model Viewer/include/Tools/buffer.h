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
#include <stdexcept>
#include <vector>

#include "globject.h"

class Buffer: public GLObject
{
public:
    Buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    ~Buffer() override;
    void bind_object() const override;
    void unbind_object() const override;
protected:
    GLenum m_target;
    GLsizeiptr m_size;
    const void* m_data;
    GLenum m_usage;
public:
    template <typename T>
    void set_buffer_data(const std::vector<T>& data)
    {
        m_data = data.data();
        m_size = data.size() * sizeof(T);
        glBufferData(m_target, m_size, m_data, m_usage);
    }

    template <typename T>
    void set_buffer_sub_data(size_t offset, const std::vector<T>& data)
    {
        if (offset + data.size() * sizeof(T) > m_size)
        {
            // Prevent out-of-bounds GPU memory write
            throw std::runtime_error("Buffer overflow: set_buffer_sub_data(...) exceeds allocated size");
        }

        m_data = data.data();
        m_size = data.size() * sizeof(T);
        glBufferSubData(m_target, offset, m_size, m_data);
    }
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

    void set_size(int size) { m_size = size; }
};

class FrameBuffer : public Buffer
{
public:
    FrameBuffer(GLsizeiptr size, const void* data, const GLuint width, const GLuint height, GLenum usage = GL_STATIC_DRAW);
    ~FrameBuffer() override;
    void bind_object() const override;
    void bind_object_numbered(GLuint fbo_id) const;
    void unbind_object() const override;
    void resize(GLuint newWidth, GLuint newHeight);

    GLuint get_width() const { return m_width; }
    GLuint get_height() const { return m_height; }
    GLuint get_render_buffer() const { return m_texture_buffer; }
    GLuint get_depth_buffer() const { return m_rbo; }

private:
    void create_framebuffer();
    void destroy_framebuffer();

    GLuint m_width;
    GLuint m_height;
    GLuint m_texture_buffer;
    GLuint m_rbo;
};

#endif