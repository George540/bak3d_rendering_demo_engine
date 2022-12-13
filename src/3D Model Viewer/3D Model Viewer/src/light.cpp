#include "light.h"
#include "shader.h"

#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <fstream>


Light::Light(glm::vec3 position, glm::vec3 scaling, Camera& camera) :
	m_position(position),
	m_scaling(scaling)
{
	mVAO = NULL;
	mVBO = NULL;

	m_properties.position = m_position;
	m_properties.diffuse = glm::vec3(1.0f);
	m_properties.ambient = glm::vec3(0.3f);
	m_properties.specular = glm::vec3(0.3f);

	build_vertex_buffer();

	m_camera = &camera;
	m_shader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LightShader.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LightShader.fs");
	std::cout << "Light created..." << std::endl;
}

Light::~Light()
{
	delete m_shader;
	delete m_camera;
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

	// Draw the triangles !
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	glBindVertexArray(0);
}

light Light::get_light_properties() const
{
	return m_properties;
}


void Light::build_vertex_buffer()
{
	//cube model
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
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindVertexArray(0);
}