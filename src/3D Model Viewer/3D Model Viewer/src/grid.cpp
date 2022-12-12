#include "grid.h"

#include <iostream>
#include <vector>
#include <glm/ext.hpp>

Grid::Grid(Camera& cam) : m_camera(&cam), number_of_slices(40), grid_size(20.0f)
{
	m_shader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/GridShader.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/GridShader.fs");

	// GRID LINE SETUP
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec4> indices;

	// Set up vertex positions
	for (int j = 0; j <= number_of_slices; ++j) {
		for (int i = 0; i <= number_of_slices; ++i) {
			// Calculate XZ plane coordinates
			const float x = static_cast<float>(i) / static_cast<float>(number_of_slices);
			const float z = static_cast<float>(j) / static_cast<float>(number_of_slices);
			vertices.emplace_back(x * grid_size - grid_size / 2.0f, 0, z * grid_size - grid_size / 2.0f);
		}
	}

	// Set up indices for Index Buffer Object for element buffer
	for (int j = 0; j < number_of_slices; ++j) {
		for (int i = 0; i < number_of_slices; ++i) {
			const int row1 = j * (number_of_slices + 1);
			const int row2 = (j + 1) * (number_of_slices + 1);

			indices.emplace_back(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1);
			indices.emplace_back(row2 + i + 1, row2 + i, row2 + i, row1 + i);

		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	number_of_elements = static_cast<GLuint>(indices.size()) * 4;

	std::cout << "Setting up grid..." << std::endl;
}

void Grid::draw() const
{
	m_shader->use();

	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());

	auto model = glm::mat4(1.0f);
	m_shader->set_mat4("model", model);

	glBindVertexArray(m_vao);
	glDrawElements(GL_LINES, number_of_elements, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

