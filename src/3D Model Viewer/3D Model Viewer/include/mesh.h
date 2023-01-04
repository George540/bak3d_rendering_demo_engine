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
    float diffuse;
    float specular;
    float ambient;
    //glm::vec3 normal;
    //float roughness;
    //texture texture;
    float shininess;
    bool useDiffuseTexture;
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