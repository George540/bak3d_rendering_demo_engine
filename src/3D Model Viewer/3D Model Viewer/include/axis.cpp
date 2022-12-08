#include "axis.h"

#include <vector>
#include <glm/ext.hpp>

Axis::Axis(Camera& camera)
{
	m_camera = &camera;
	m_shader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LineShader.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LineShader.fs");
	constexpr float vertices[] = {0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f};

	// Create a vertex array
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(0);
}

void Axis::draw() const
{
	m_shader->use();

	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());

	auto model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f, 100.0f, 1.0f));
	m_shader->set_mat4("model", model);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}
