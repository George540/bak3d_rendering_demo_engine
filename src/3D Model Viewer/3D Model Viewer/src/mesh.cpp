#include "mesh.h"

#include <utility>

Mesh::Mesh(vector<vertex> vertices, vector<unsigned> indices, vector<texture> textures) :
    vertices(std::move(vertices)),
    indices(std::move(indices)),
    textures(std::move(textures))
{
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    set_up_mesh();
}

void Mesh::draw(const Shader& shader) const
{
    // bind appropriate textures
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    unsigned int normal_nr = 1;
    unsigned int height_nr = 1;

    for (auto i = 0; i < textures.size(); ++i)
    {
        // active proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuse_nr++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specular_nr++);
        }
        else if (name == "texture_normal")
        {
            number = std::to_string(normal_nr++);
        }
        else if (name == "texture_height")
        {
            number = std::to_string(height_nr++);
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.get_id(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::set_up_mesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), static_cast<void*>(nullptr));
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex_coords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(vertex), (void*)offsetof(vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_Weights));
    glBindVertexArray(0);
}