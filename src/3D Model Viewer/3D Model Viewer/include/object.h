#ifndef OBJECT_H
#define OBJECT_H

#include <glad.h>
#include <string>
#include <glm/glm.hpp>

class Object
{
	glm::vec3 m_position;
	glm::vec3 m_scaling;
	glm::mat4 m_world_transformation_matrix;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_num_of_vertices;
public:
	Object();
	Object(glm::vec3& position, glm::vec3& scale);
	~Object();

	void draw();
	void set_up_model_buffer(std::string path, GLuint& vertex_count);
	void update_world_matrix();
	void build_test_object(GLuint& vertex_count);
};

#endif