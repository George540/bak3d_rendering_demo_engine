#include <iostream>
#include <fstream>
#include <filesystem>

#include "light.h"
#include "shader.h"
#include "event_manager.h"
#include "renderer.h"

Light::Light(glm::vec3 position, glm::vec3 scaling, Camera& camera) :
	m_position(position),
	m_scaling(scaling),
	m_camera(&camera)
{
	m_vao = NULL;
	m_vbo = NULL;

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


	build_vertex_buffer();

	m_shader = new Shader(
		std::filesystem::absolute("shaders/LightShader.vs").string().c_str(),
		std::filesystem::absolute("shaders/LightShader.fs").string().c_str());
	std::cout << "Light created..." << std::endl;
}

Light::~Light()
{
	delete m_shader;
	delete m_camera;
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
}


void Light::draw() const
{
	m_shader->use();

	// MVP Update
	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());
	auto model = glm::mat4(1.0f);
	model = translate(model, m_position) * scale(model, m_scaling);
	m_shader->set_mat4("model", model);

	m_shader->set_vec4("diffuseColor", glm::vec4(m_properties.diffuse, 1.0f));

	// Draw the triangles !
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	glBindVertexArray(0);
}

void Light::build_vertex_buffer()
{
	//cube model, since it's just a cube, no need to load model
	constexpr float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	// Create a vertex array
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindVertexArray(0);
}

void Light::delete_vao_vbo() const
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}
