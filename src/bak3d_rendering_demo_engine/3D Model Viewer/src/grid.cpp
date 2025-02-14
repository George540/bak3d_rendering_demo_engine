#include <iostream>
#include <vector>
#include <filesystem>

#include <glm/ext.hpp>

#include "grid.h"

Grid::Grid(Camera& cam, Shader* shader) : Object(cam, shader), m_number_of_slices(40), m_grid_size(20.0f)
{
	// GRID LINE SETUP
	// Grid setup used indices and EBOs for proper identification of each line
	// Part of the solution is referenced from thewoz's answer on stackoverflow.com
	// Link to question and answer: https://stackoverflow.com/questions/58494179/how-to-create-a-grid-in-opengl-and-drawing-it-with-lines#:~:text=this%20the%20code%20for%20render,opengl
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec4> indices;

	// Set up vertex positions
	for (int j = 0; j <= m_number_of_slices; ++j) {
		for (int i = 0; i <= m_number_of_slices; ++i) {
			// Calculate XZ plane coordinates
			const float x = static_cast<float>(i) / static_cast<float>(m_number_of_slices);
			const float z = static_cast<float>(j) / static_cast<float>(m_number_of_slices);
			vertices.emplace_back(x * m_grid_size - m_grid_size / 2.0f, 0, z * m_grid_size - m_grid_size / 2.0f);
		}
	}

	// Set up indices for Index Buffer Object for element buffer
	for (int j = 0; j < m_number_of_slices; ++j) {
		for (int i = 0; i < m_number_of_slices; ++i) {
			const int row1 = j * (m_number_of_slices + 1);
			const int row2 = (j + 1) * (m_number_of_slices + 1);

			indices.emplace_back(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1);
			indices.emplace_back(row2 + i + 1, row2 + i, row2 + i, row1 + i);
		}
	}

	m_vbo = new VertexBuffer(sizeof(glm::vec3) * vertices.size(), &vertices[0]);
	m_ebo = new ElementBuffer(sizeof(glm::uvec4) * indices.size(), &indices[0]);

	m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	m_vao->set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	m_vao->unbind_object();

	m_number_of_elements = static_cast<GLuint>(indices.size()) * 4;

	std::cout << "Setting up grid..." << std::endl;
}

void Grid::draw() const
{
	Object::draw();

	m_shader->set_vec3("color", glm::vec3(0.5f));

	m_vao->bind_object();
	glDrawElements(GL_LINES, m_number_of_elements, GL_UNSIGNED_INT, nullptr);
	m_vao->unbind_object();
}