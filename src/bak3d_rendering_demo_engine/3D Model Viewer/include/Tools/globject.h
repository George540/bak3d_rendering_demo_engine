// =====================================================================================
// GLObject class was made by George Mavroeidis as an abstract class for opengl objects.
// It contains a set of virtual and pure virtual functions that can be implemented in
// subclasses and derived objects. It can be used by all opengl objects that bind data.
// =====================================================================================

#pragma once

#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <glad/glad.h>

class GLObject
{
public:
    virtual ~GLObject() {}
    virtual void bind_object() const = 0;
    virtual void unbind_object() const = 0;
    GLuint get_id() const { return m_ID; }

protected:
    GLuint m_ID = 0;
};

#endif