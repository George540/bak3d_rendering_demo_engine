#include "mesh.h"

#include <iostream>
#include <utility>

#include "resource_manager.h"

Mesh::Mesh(Camera& cam, vector<Vertex> vertices, vector<GLuint> indices, vector<Texture2D> textures) :
    Object(cam, *ResourceManager::get_shader("ModelShader")),
	m_vertices(std::move(vertices)),
	m_indices(std::move(indices)),
	m_textures(std::move(textures))
{
    // create buffers/arrays
    m_vbo = new VertexBuffer(sizeof(Vertex) * m_vertices.size(), m_vertices.data());
    m_ebo = new ElementBuffer(sizeof(GLuint) * m_indices.size(), m_indices.data());

    m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));
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
    //glBindTexture(GL_TEXTURE_2D, 0);
    vector<GLuint> texture_ids(m_textures.size());
    for (int i = 0; i < m_textures.size(); ++i)
    {
        texture_ids[i] = m_textures[i].get_id();
    }
    glDeleteTextures(static_cast<GLsizei>(m_textures.size()), texture_ids.data());
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
        auto type = m_textures[i].get_texture_type();
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
            m_textures[i].bind();
        }
    }

    // draw mesh
    m_vao->bind_object();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();

    // always good practice to set everything back to defaults once configured.
    for (auto i = 0; i < m_textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        m_textures[i].unbind();
    }
    glActiveTexture(GL_TEXTURE0);
}