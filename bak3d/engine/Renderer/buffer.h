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

#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
#include <stdexcept>
#include <vector>

#include "globject.h"

/*
 * Base class for OpenGL buffer OpenGL objects that contains data to be passed to the GPU.
 */
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

/*
 * Buffer subclass for storing array buffer data
 */
class VertexBuffer : public Buffer
{
public:
    VertexBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ARRAY_BUFFER, size, data, usage) {}
};

/*
 * Buffer subclass for storing element buffer data
 */
class ElementBuffer : public Buffer
{
public:
    ElementBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ELEMENT_ARRAY_BUFFER, size, data, usage) {}
};

/*
 * Buffer subclass for storing instanced buffer data for instanced objects
 */
class InstanceBuffer : public Buffer
{
public:
    InstanceBuffer(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW)
        : Buffer(GL_ARRAY_BUFFER, size, data, usage) {}

    void set_size(int size) { m_size = size; }
};

/*
 * Framebuffer class that stores generated output textures with rendering buffer objects.
 * Default framebuffer for OpenGL has ID 0, which is rendering a texture in the full viewport window.
 */
class FrameBuffer : public Buffer
{
public:
    FrameBuffer(GLsizeiptr size, const void* data, const GLuint width, const GLuint height, GLenum usage = GL_STATIC_DRAW);
    ~FrameBuffer() override;
    void bind_object() const override;
    void unbind_object() const override;
    void resize(GLuint newWidth, GLuint newHeight);

    GLuint get_width() const { return m_width; }
    GLuint get_height() const { return m_height; }
    float get_aspect_ratio() const { return m_width / m_height; }
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