#include <filesystem>

#include "object.h"

Object::Object(Camera& camera) : Object(camera, "LineShader")
{
	
}

Object::Object(Camera& camera, const string shader_name)
{
	m_camera = &camera;
	m_shader = new Shader(
		std::filesystem::absolute("shaders/" + shader_name + ".vert").string().c_str(),
		std::filesystem::absolute("shaders/" + shader_name + ".frag").string().c_str());

	m_vao = new VertexArray();
	m_vbo = nullptr; // properly assign in subclasses
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