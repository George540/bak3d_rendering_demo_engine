#include "buffer.h"

Buffer::Buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
    : m_Target(target)
{
    // TODO: make constructor arguments more defined
    glGenBuffers(1, &m_ID);
    glBindBuffer(m_Target, m_ID);
    glBufferData(m_Target, size, data, usage);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_ID);
}

void Buffer::bind_object() const
{
    glBindBuffer(m_Target, m_ID);
}

void Buffer::unbind_object() const
{
    glBindBuffer(m_Target, 0);
}