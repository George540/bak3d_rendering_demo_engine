#include <vector>
#include <iostream>

#include "scene.h"
#include "renderer.h"
#include <glm/gtc/type_ptr.hpp>


using namespace std;

World* World::instance;

World::World()
{
	instance = this;

	// Camera Setup


}

World::~World()
{
	// delete camera
}

void World::update(float dt)
{
	// camera
    cout << "\rCurrent frame dt: " << dt << ' ' << flush;
}

void World::draw()
{
    Renderer::begin_frame();
    //generate_grid();
    Renderer::end_frame();
}

void World::generate_grid()
{
    auto slices = 10;
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec4> indices;

    for (int j = 0; j <= slices; ++j)
    {
        for (int i = 0; i <= slices; ++i)
        {
            float x = (float)i / (float)slices;
            float y = 0;
            float z = (float)j / (float)slices;
            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    for (int j = 0; j < slices; ++j)
    {
        for (int i = 0; i < slices; ++i)
        {
            int row1 = j * (slices + 1);
            int row2 = (j + 1) * (slices + 1);

            indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
            indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));

        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), &vertices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //lenght = (GLuint)indices.size() * 4;
}

