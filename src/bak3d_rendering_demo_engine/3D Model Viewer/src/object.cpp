#include <filesystem>

#include "object.h"

using namespace std;

Object::Object(Material* material)
{
	m_material = material;

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
	delete m_material;
	delete m_camera;
}

void Object::update(float dt)
{
	set_model_matrix(glm::vec3(0.0f), m_scaling, m_euler_rotation, 0.0f);
}

void Object::draw() const
{
	if (!m_material || !m_camera) return;
	
	m_material->set_mat4("projection", m_camera->get_projection_matrix());
	m_material->set_mat4("view", m_camera->get_view_matrix());
	m_material->set_mat4("model", m_model_matrix);

	m_material->apply();
}

InstancedObject::InstancedObject(Material* material) : Object(material)
{
	// Will be handled by derived class
	m_ibo = nullptr;
}

InstancedObject::~InstancedObject()
{
	delete m_ibo;
}