#include "mesh.h"

#include <iostream>
#include <utility>

#include "resource_manager.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<string> textures) :
    Object(*ResourceManager::get_shader("ModelShader")),
	m_vertices(move(vertices)),
	m_indices(move(indices)),
    m_textures(move(textures))
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
    m_textures.clear();
}

void Mesh::draw() const
{
    Object::draw();

    // bind appropriate textures
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    unsigned int normal_nr = 1;
    unsigned int height_nr = 1;

    for (auto i = 0; i < m_textures.size(); ++i)
    {
        // active proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // retrieve texture number (the N in diffuse_textureN)
        auto texture = ResourceManager::get_texture(m_textures[i]);
        auto type = texture->get_texture_type();
        string number;
        string name;
        int nr;
        if (type == aiTextureType_DIFFUSE)
        {
            number = std::to_string(diffuse_nr++);
            name = "diffuse";
            nr = diffuse_nr;
        }
        else if (type == aiTextureType_SPECULAR)
        {
            number = std::to_string(specular_nr++);
            name = "specular";
            nr = specular_nr;
        }
        else if (type == aiTextureType_NORMALS)
        {
            number = std::to_string(normal_nr++);
            name = "normal";
            nr = normal_nr;
        }
        else if (type == aiTextureType_HEIGHT)
        {
            number = std::to_string(height_nr++);
            name = "height";
            nr = height_nr;
        }
        
        if (nr > 1)
        {
            texture->bind();
        }
    }

    // draw mesh
    m_vao->bind_object();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}