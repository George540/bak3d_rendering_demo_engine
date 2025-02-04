// =====================================================================================
// A collection of mesh data structs and classes made by George Mavroeidis for setting up
// drawing meshes. Contains building blocks
// =====================================================================================

#pragma once

#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

constexpr auto MAX_BONE_INFLUENCE = 4;

// TODO: Turn into proper separate classes with all necessary methods
struct Vertex
{
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

struct Edge
{
    GLuint v1_index;
    GLuint v2_index;

    Edge(unsigned int a, unsigned int b) : v1_index(a), v2_index(b)
    {
        if (v1_index > v2_index)
        {
            std::swap(v1_index, v2_index); // Ensure consistent ordering
        }
    }

    // Comparison operator for set uniqueness
    bool operator<(const Edge& other) const
    {
        return v1_index < other.v1_index || (v1_index == other.v1_index && v2_index < other.v2_index);
    }
};

struct Material
{
    float diffuse;
    float specular;
    float ambient;
    float shininess;
    bool use_diffuse_texture;
    bool use_specular_texture;
    bool use_normal_texture;
};

#endif
