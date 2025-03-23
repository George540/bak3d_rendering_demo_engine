#include "mesh.h"

#include <iostream>
#include <utility>

#include "resource_manager.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices) :
    Object(nullptr), // no initial material at first
	m_vertices(std::move(vertices)),
	m_indices(std::move(indices))
{
    // create buffers/arrays
    m_vbo = new VertexBuffer(sizeof(Vertex) * m_vertices.size(), m_vertices.data());
    m_ebo = new ElementBuffer(sizeof(GLuint) * m_indices.size(), m_indices.data());

    m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    m_vao->set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    m_vao->set_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coords)));
    m_vao->set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
    m_vao->set_attrib_pointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));
    m_vao->set_attrib_pointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_BoneIDs)));
    m_vao->set_attrib_pointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_Weights)));

    m_vao->unbind_object();
}

Mesh::~Mesh()
{
    //m_textures.clear();
}

void Mesh::draw() const
{
    Object::draw();

    // draw mesh
    m_vao->bind_object();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}