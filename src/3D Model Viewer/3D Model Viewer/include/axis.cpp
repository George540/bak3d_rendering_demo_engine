#include "axis.h"

#include <iostream>
#include <vector>
#include <glm/ext.hpp>

#include "renderer.h"

Axis::Axis(Camera& camera)
{
	m_camera = &camera;
	m_shader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LineShader.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LineShader.fs");
	constexpr glm::vec3 vertices[] = { // position, color
									glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), // X red line
									glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),

									glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), // Y green line
									glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),

									glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), // Z blue line
									glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
									};

	// Create a vertex array
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(0);

	std::cout << "Setting up 3D axis gizmo..." << std::endl;
}

Axis::~Axis()
{
	delete m_shader;
	delete m_camera;
}

void Axis::draw() const
{
	m_shader->use();

	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());

	auto model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f));
	m_shader->set_mat4("model", model);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
}
