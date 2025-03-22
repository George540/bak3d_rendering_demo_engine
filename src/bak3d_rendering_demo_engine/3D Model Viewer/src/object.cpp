#include <filesystem>

#include "object.h"

Object::Object(Shader* shader)
{
	m_shader = shader;

	m_vao = new VertexArray();
	m_vao->bind_object();

	// Will be handled by derived class
	m_vbo = nullptr;
	m_ebo = nullptr;
}

Object::~Object()
{
	delete m_vao;
	delete m_vbo;
	delete m_ebo;
	delete m_shader;
	delete m_camera;
}

void Object::update(float dt)
{
	set_model_matrix(glm::vec3(0.0f), m_scaling, m_euler_rotation, 0.0f);
}

void Object::draw() const
{
	if (!m_shader || !m_camera) return;

	m_shader->use();

	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());
	m_shader->set_mat4("model", m_model_matrix);
}

void Object::delete_globjects() const
{
	delete m_vao;
	delete m_vbo;
}

/*
IndexedObject::IndexedObject(Camera& camera) : IndexedObject(camera, "LineShader")
{

}

IndexedObject::IndexedObject(Camera& camera, const string shader_name) : Object(camera, shader_name)
{
	m_ebo = new ElementBuffer()
}
*/