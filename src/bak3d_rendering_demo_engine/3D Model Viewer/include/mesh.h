// =====================================================================================
// This mesh class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#pragma once

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "mesh_data.h"
#include "object.h"
#include "shader.h"
#include "texture.h"

class Mesh : public Object
{
public:
    // mesh Data
    Material m_material{};
    vector<Vertex> m_vertices;
    vector<GLuint> m_indices;
    vector<std::string> m_textures;

    // constructor
    Mesh(Camera& cam, vector<Vertex> vertices, vector<GLuint> indices, vector<string> textures);
    ~Mesh() override;
    void draw() const override;

    Shader* get_current_shader() const { return m_shader; }
    void set_current_shader(Shader* shader) { m_shader = shader; }
};

#endif