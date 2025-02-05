#include <iostream>
#include <fstream>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>

#include "light.h"
#include "Tools/primitive_mesh_data.h"

#include "event_manager.h"
#include "renderer.h"

Light::Light(glm::vec3 position, glm::vec3 scaling, Camera& camera) :
	Object(camera, "LightShader")
{
	m_position = position;
	m_scaling = scaling;
	m_vao->bind_object();

	m_vbo = new VertexBuffer(sizeof(glm::vec3) * CUBE_VERTICES_SOLID.size(), &CUBE_VERTICES_SOLID[0]);
	m_ebo = new ElementBuffer(sizeof(GLuint) * CUBE_INDICES_SOLID.size(), &CUBE_INDICES_SOLID[0]);

	m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	m_vao->unbind_object();

	m_properties.position = m_position;
	m_properties.diffuse = glm::vec3(1.0f);
	m_properties.ambient = glm::vec3(0.3f);
	m_properties.specular = glm::vec3(0.3f);

	m_horizontal_angle = m_position.x;
	m_vertical_angle = m_position.y;
	m_distance_offset = glm::distance(m_position, glm::vec3(0.0f));

	Renderer::light_horizontal_rotation = 45.0f;
	Renderer::light_vertical_rotation = 135.0f;
	Renderer::light_origin_distance = m_distance_offset;

	std::cout << "Light created..." << std::endl;
}

void Light::update(float dt)
{
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_horizontal_angle += dt * 30.0f;
	}
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_horizontal_angle -= dt * 30.0f;
	}

	const auto theta = glm::radians(m_horizontal_angle);
	const auto phi = glm::radians(m_vertical_angle);
	m_position = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_distance_offset += dt * 30.0f;
	}
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_distance_offset -= dt * 30.0f;
	}
	m_position *= m_distance_offset;
	m_properties.position = m_position;

	set_model_matrix(m_position, get_scaling(), get_rotation(), 0.0f);
}


void Light::draw() const
{
	Object::draw();

	m_shader->set_vec4("diffuseColor", glm::vec4(m_properties.diffuse, 1.0f));

	m_vao->bind_object();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(CUBE_INDICES_SOLID.size()), GL_UNSIGNED_INT, nullptr);
	//glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	m_vao->unbind_object();
}
