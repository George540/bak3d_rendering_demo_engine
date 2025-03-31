#include "buffer.h"

Buffer::Buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage) :
    m_target(target),
    m_size(size),
    m_data(data),
    m_usage(usage)
{
    // TODO: make constructor arguments more defined
    glGenBuffers(1, &m_ID);
    glBindBuffer(m_target, m_ID);
    glBufferData(m_target, m_size, m_data, m_usage);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_ID);
}

void Buffer::bind_object() const
{
    glBindBuffer(m_target, m_ID);
}

void Buffer::buffer_subdata() const
{
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, m_data);
}

void Buffer::unbind_object() const
{
    glBindBuffer(m_target, 0);
}