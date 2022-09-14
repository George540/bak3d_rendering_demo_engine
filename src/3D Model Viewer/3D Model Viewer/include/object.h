#ifndef OBJECT_H
#define OBJECT_H

#include <glad.h>
#include <glm/glm.hpp>

class Object
{
	glm::vec3 m_position;
	glm::vec3 m_scaling;
	glm::mat4 m_world_transformation_matrix;
	glm::vec4 m_color;

	GLuint m_vbo;
	GLuint m_vao;
public:
	Object();
	Object(glm::vec3& position, glm::vec3& scale, glm::vec4& color);
};

#endif

