// =====================================================================================
// This mesh class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "shader.h"

#define MAX_BONE_INFLUENCE 4

struct vertex {
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 tex_coords;
    // tangent
    glm::vec3 tangent;
    // bitangent
    glm::vec3 bitangent;
    // color
    glm::vec3 color;
    // use diffuse texture or not
    bool useDiffuseTexture;
    // use specular texture or not
    bool useSpecularTexture;
    // use normal texture or not
    bool useNormalsTexture;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct texture {
    GLuint id;
    string type;
    string path;
};

struct material {
    float diffuse;
    float specular;
    float ambient;
    float shininess;
    bool use_diffuse_texture;
    bool use_specular_texture;
    bool use_normal_texture;
};

class Mesh
{
public:
    // mesh Data
    material m_material{};
    vector<vertex> vertices;
    vector<unsigned int> indices;
    vector<texture> textures;

    // constructor
    Mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<texture> textures);
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