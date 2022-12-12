#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>

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
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct texture {
    unsigned int id;
    string type;
    string path;
};

struct material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    glm::vec3 Normal;
    glm::vec3 Roughness;
    float Shininess;
};

class Mesh
{
public:
    // mesh Data
    material m_material;
    vector<vertex> vertices;
    vector<unsigned int> indices;
    vector<texture> textures;
    unsigned int m_vao;

    // constructor
    Mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<texture> textures);
    void draw(const Shader& shader) const; // render the mesh
private:
    // render data 
    unsigned int m_vbo, m_ebo;
    void set_up_mesh();
};

#endif