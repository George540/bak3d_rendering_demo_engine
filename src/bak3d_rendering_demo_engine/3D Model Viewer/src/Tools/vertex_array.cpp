#include "vertex_array.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::bind_object() const
{
    glBindVertexArray(m_ID);
}

void VertexArray::unbind_object() const
{
    glBindVertexArray(0);
}

void VertexArray::set_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer, GLuint divisor)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);

    if (divisor > 0)
    {
        glVertexAttribDivisor(index, divisor);
    }
}