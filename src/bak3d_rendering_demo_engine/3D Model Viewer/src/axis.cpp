#include <iostream>

#include "axis.h"
#include "../src/Tools/primitive_mesh_data.h"

#include "renderer.h"

Axis::Axis(Shader* shader) :
	Object(shader)
{
	m_scaling = glm::vec3(3.0f);
	set_model_matrix(glm::vec3(0.0f), m_scaling, m_euler_rotation, 0.0f);

	m_vbo = new VertexBuffer(sizeof(AxisVertex) * AXIS_VERTICES.size(), &AXIS_VERTICES[0]);
	m_ebo = new ElementBuffer(sizeof(GLuint) * AXIS_INDICES.size(), &AXIS_INDICES[0]);

	m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), reinterpret_cast<void*>(offsetof(AxisVertex, position)));
	m_vao->set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), reinterpret_cast<void*>(offsetof(AxisVertex, color)));
	m_vao->unbind_object();

	std::cout << "Setting up 3D axis gizmo..." << std::endl;
}

void Axis::draw() const
{
	Object::draw();

	m_vao->bind_object();
	glDrawElements(GL_LINES, static_cast<GLsizei>(AXIS_INDICES.size()), GL_UNSIGNED_INT, nullptr);
	m_vao->unbind_object();
}