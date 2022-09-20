#include "object.h"
#include "OBJloader.h"

#include <iostream>
#include <vector>
#include <glm/ext/matrix_transform.hpp>

using namespace std;

Object::Object()
{
	m_vao = NULL;

	m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_scaling = glm::vec3(1.0f, 1.0f, 1.0f);

	cout << "Setting up object..." << endl;
	auto path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/untitled.obj";
	set_up_model_buffer(path, m_num_of_vertices);

	cout << "Object with default settings spawned." << endl;
}

Object::Object(glm::vec3& position, glm::vec3& scale) :
	m_position(position),
	m_scaling(scale)
{
	m_vao = NULL;

	auto path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/untitled.obj";
	set_up_model_buffer(path, m_num_of_vertices);

	cout << "Object spawned." << endl;
}

Object::~Object()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void Object::set_up_model_buffer(std::string path, GLuint& vertex_count)
{
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;

	//read the vertex data from the model's OBJ file
	loadOBJ(path.c_str(), vertices, normals, uvs);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Vertex VBO setup
	GLuint vertices_vbo;
	glGenBuffers(1, &vertices_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_vbo;
	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_vbo;
	glGenBuffers(1, &uvs_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	m_vao = vao;
	vertex_count = vertices.size();
}

void Object::draw()
{
	//update_world_matrix();

	// Draw the Vertex Buffer
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vao);

	//Bind Textures
	//...

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, m_num_of_vertices);
}


void Object::update_world_matrix()
{
	auto model = glm::mat4(1.0f);
	m_world_transformation_matrix = translate(model, m_position) * rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * scale(model, m_scaling);
}