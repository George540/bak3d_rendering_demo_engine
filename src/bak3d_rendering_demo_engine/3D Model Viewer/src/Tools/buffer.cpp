#include "buffer.h"

#include <iostream>

using namespace std;

Buffer::Buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage) :
    m_target(target),
    m_size(size),
    m_data(data),
    m_usage(usage)
{
    // TODO: make constructor arguments more defined
    if (m_usage != GL_NONE)
    {
        glGenBuffers(1, &m_ID);
        glBindBuffer(m_target, m_ID);
        glBufferData(m_target, m_size, m_data, m_usage);
    }
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_ID);
}

void Buffer::bind_object() const
{
    glBindBuffer(m_target, m_ID);
}

void Buffer::unbind_object() const
{
    glBindBuffer(m_target, 0);
}

FrameBuffer::FrameBuffer(GLsizeiptr size, const void* data, const GLuint width, const GLuint height, GLenum usage)
    : Buffer(GL_FRAMEBUFFER, size, data, usage),
    m_width(width),
    m_height(height)
{
    cout << "Enabling Frame Buffer Object..." << endl;
    create_framebuffer();
}

FrameBuffer::~FrameBuffer()
{
    destroy_framebuffer();
}

void FrameBuffer::bind_object() const
{
    glBindFramebuffer(m_target, m_ID);
    glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::unbind_object() const // when unbinding a frame buffer, it binds back to default
{
    glBindFramebuffer(m_target, 0);
    glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::resize(GLuint newWidth, GLuint newHeight)
{
    if (m_width != newWidth || m_height != newHeight)
    {
        m_width = newWidth;
        m_height = newHeight;

        destroy_framebuffer();
        create_framebuffer();
    }
}

void FrameBuffer::create_framebuffer()
{
    if (m_target != GL_FRAMEBUFFER)
    {
        cout << "ERROR: Framebuffer target is not correct. Must be GL_FRAMEBUFFER" << endl;
    }

    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(m_target, m_ID);

    glGenTextures(1, &m_texture_buffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(m_target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_buffer, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(m_target, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    // Verify framebuffer is complete
    if (GLenum status = glCheckFramebufferStatus(m_target); status != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR: Framebuffer is not complete! Status: 0x%x\n" <<  status << endl;
    }

    glBindFramebuffer(m_target, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::destroy_framebuffer()
{
    if (m_ID != 0)
    {
        glDeleteFramebuffers(1, &m_ID);
        m_ID = 0;
    }
    if (m_texture_buffer != 0)
    {
        glDeleteTextures(1, &m_texture_buffer);
        m_texture_buffer = 0;
    }
    if (m_rbo != 0)
    {
        glDeleteRenderbuffers(1, &m_rbo);
        m_rbo = 0;
    }
}
