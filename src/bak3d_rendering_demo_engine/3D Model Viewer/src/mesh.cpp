#include "mesh.h"

#include <iostream>
#include <utility>

#include "resource_manager.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<string> textures) :
    Object(nullptr),
	m_vertices(std::move(vertices)),
	m_indices(std::move(indices)),
    m_textures(std::move(textures))
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

    for (const auto& texture_string : m_textures)
    {
        Texture2D* texture = ResourceManager::get_texture(texture_string);
        
        if (texture->get_texture_type() == aiTextureType_DIFFUSE && !m_material.diffuse)
        {
            m_material.diffuse = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_SPECULAR && !m_material.specular)
        {
            m_material.specular = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_NORMALS && !m_material.normal)
        {
            m_material.normal = texture;
        }
        else if (texture->get_texture_type() == aiTextureType_HEIGHT && !m_material.height)
        {
            m_material.height = texture;
        }
    }

    if (!m_material.diffuse) m_material.diffuse = ResourceManager::get_texture("None.jpg");
    if (!m_material.specular) m_material.specular = ResourceManager::get_texture("None.jpg");
    if (!m_material.normal) m_material.normal = ResourceManager::get_texture("None.jpg");
    if (!m_material.height) m_material.height = ResourceManager::get_texture("None.jpg");
}

Mesh::~Mesh()
{
    m_textures.clear();
}

void Mesh::draw() const
{
    m_shader->set_int("material.diffuse", m_material.diffuse->get_asset_id());
    m_material.diffuse->bind();
    
    m_shader->set_int("material.specular", m_material.specular->get_asset_id());
    m_material.specular->bind();
    
    m_shader->set_int("material.normal", m_material.normal->get_asset_id());
    m_material.normal->bind();
    
    m_shader->set_int("material.height", m_material.height->get_asset_id());
    //m_material.height->bind();

    Object::draw();

    // bind appropriate textures
    /*unsigned int diffuse_nr = 0;
    unsigned int specular_nr = 0;
    unsigned int normal_nr = 0;
    unsigned int height_nr = 0;
    */

    /*Texture2D* diffuse_texture = nullptr;
    Texture2D* specular_texture = nullptr;
    Texture2D* normal_texture = nullptr;
    Texture2D* height_texture = nullptr;

    for (const auto& m_texture : m_textures)
    {
        // retrieve texture number (the N in diffuse_textureN)
        auto texture = ResourceManager::get_texture(m_texture);
        auto type = texture->get_texture_type();
        
        if (type == aiTextureType_DIFFUSE && !diffuse_texture)
        {
            diffuse_texture = texture;
            //m_shader->set_int("material.diffuse", texture->get_asset_id());
        }
        else if (type == aiTextureType_SPECULAR && !specular_texture)
        {
            specular_texture = texture;
            //m_shader->set_int("material.specular", texture->get_asset_id());
        }
        else if (type == aiTextureType_NORMALS && !normal_texture)
        {
            normal_texture = texture;
            //m_shader->set_int("material.normal", texture->get_asset_id());
        }
        else if (type == aiTextureType_HEIGHT && !height_texture)
        {
            height_texture = texture;
            //m_shader->set_int("material.height", texture->get_asset_id());
        }
        
        //texture->bind();
    }

    if (!diffuse_texture)
    {
        diffuse_texture = ResourceManager::get_texture("None.jpg");
    }
    if (!specular_texture)
    {
        specular_texture = ResourceManager::get_texture("None.jpg");
    }
    if (!normal_texture)
    {
        normal_texture = ResourceManager::get_texture("None.jpg");
    }
    if (!height_texture)
    {
        height_texture = ResourceManager::get_texture("None.jpg");
    }
    m_shader->set_int("material.diffuse", static_cast<int>(diffuse_texture->get_asset_id()));
    diffuse_texture->bind();
    m_shader->set_int("material.specular", static_cast<int>(specular_texture->get_asset_id()));
    specular_texture->bind();
    m_shader->set_int("material.normal", static_cast<int>(normal_texture->get_asset_id()));
    normal_texture->bind();
    m_shader->set_int("material.height", static_cast<int>(height_texture->get_asset_id()));
    height_texture->bind();*/

    // draw mesh
    m_vao->bind_object();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    m_vao->unbind_object();

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}