#pragma once

#include <glad/glad.h>
#include <vector>
#include <glm/vec3.hpp>

// Unique cube vertices (8 total)
std::vector<glm::vec3> CUBE_VERTICES_SOLID =
{
    {-0.5f, -0.5f, -0.5f}, // 0
    { 0.5f, -0.5f, -0.5f}, // 1
    { 0.5f,  0.5f, -0.5f}, // 2
    {-0.5f,  0.5f, -0.5f}, // 3
    {-0.5f, -0.5f,  0.5f}, // 4
    { 0.5f, -0.5f,  0.5f}, // 5
    { 0.5f,  0.5f,  0.5f}, // 6
    {-0.5f,  0.5f,  0.5f}  // 7
};

// Indices for the Element Buffer Object (EBO) - 12 triangles (36 indices)
std::vector<GLuint> CUBE_INDICES_SOLID =
{
    0, 1, 2, 2, 3, 0,  // Front face
    1, 5, 6, 6, 2, 1,  // Right face
    5, 4, 7, 7, 6, 5,  // Back face
    4, 0, 3, 3, 7, 4,  // Left face
    3, 2, 6, 6, 7, 3,  // Top face
    4, 5, 1, 1, 0, 4   // Bottom face
};

// Vector of glm::vec3 for cube vertices
std::vector<glm::vec3> CUBE_VERTICES_EDGED =
{
    {-0.5f, -0.5f, -0.5f},  // 0
    { 0.5f, -0.5f, -0.5f},  // 1
    { 0.5f,  0.5f, -0.5f},  // 2
    {-0.5f,  0.5f, -0.5f},  // 3
    {-0.5f, -0.5f,  0.5f},  // 4
    { 0.5f, -0.5f,  0.5f},  // 5
    { 0.5f,  0.5f,  0.5f},  // 6
    {-0.5f,  0.5f,  0.5f}   // 7
};

// Vector of GLuint for cube indices (edges)
std::vector<GLuint> CUBE_INDICES_EDGED =
{
    0, 1,  1, 2,  2, 3,  3, 0,  // Bottom face edges
    4, 5,  5, 6,  6, 7,  7, 4,  // Top face edges
    0, 4,  1, 5,  2, 6,  3, 7   // Vertical edges
};

// Unique vertices for the line (only position)
std::vector<glm::vec3> LINE_VERTICES =
{
    {-1.0f, 0.0f, 0.0f}, // 0: Start point
    {1.0f, 0.0f, 0.0f}   // 1: End point
};

// Indices for the Element Buffer Object (EBO)
std::vector<GLuint> LINE_INDICES =
{
    0, 1  // Single line connecting two points
};