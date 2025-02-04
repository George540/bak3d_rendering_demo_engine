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

	m_vao = new VertexArray();
	m_vao->bind_object();

	m_vbo = new VertexBuffer(sizeof(vertices), vertices);
	m_vao->set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	m_vbo->bind_object();
	m_vao->unbind_object();
}

void Object::draw() const
{
	m_shader->use();

	m_shader->set_mat4("projection", m_camera->get_projection_matrix());
	m_shader->set_mat4("view", m_camera->get_view_matrix());
	
	// DON'T FORGET model matrix
	// Follow up with other uniforms
	// binding, drawing and unbinding
	// TODO: add translation, rotation and scaling based on Transform class
}

void Object::delete_globjects() const
{
	delete m_vao;
	delete m_vbo;
}