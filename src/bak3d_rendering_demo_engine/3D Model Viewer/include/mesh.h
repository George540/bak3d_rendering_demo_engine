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
#include "shader.h"
#include "texture.h"

class Mesh
{
public:
    // mesh Data
    Material m_material{};
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture2D> textures;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture2D> textures);
    void draw(const Shader& shader) const; // render the mesh
    void delete_vao_vbo() const;
private:
    // render data 
    GLuint m_vao{};
    GLuint m_vbo{};
	GLuint m_ebo{};
    void set_up_mesh();
};

#endif