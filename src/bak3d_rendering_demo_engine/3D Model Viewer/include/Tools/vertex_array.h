// =====================================================================================
// VertexArray class was made by George Mavroeidis as a utility class for using VAOs.
// It contains a set of data that are loaded in the constructor of the class and stored
// within the declared object. It can be used by all objects that use vertex arrays.
// =====================================================================================

#pragma once

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>
#include <vector>

#include "globject.h"

class VertexArray : public GLObject
{
public:
    VertexArray();
    ~VertexArray() override;

    void bind_object() const override;
    void unbind_object() const override;

    void set_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer, GLuint divisor = 0);
};

#endif