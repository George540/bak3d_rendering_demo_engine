#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <vector>
#include <glad.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"

struct light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class Light
{
	// ORIENTATION
	glm::vec3 m_position;
	glm::vec3 m_scaling;

	// MESH
	unsigned int mVAO;
	unsigned int mVBO;
	std::vector<glm::vec3> mVertexPositions;

	// EXTERNAL
	Shader* m_shader;
	Camera* m_camera;

	// PROPERTIES
	light m_properties{};

public:
	Light(glm::vec3 position, glm::vec3 scaling, Camera& camera);
	~Light();

	void draw() const;
	light get_light_properties() const { return m_properties; }
	void set_position(const glm::vec3& position) { m_position = position; }
	void set_scaling(const glm::vec3& scaling) { m_scaling = scaling; }
	glm::vec3 get_position() const { return m_position; }
	glm::vec3 get_scaling() const { return m_scaling; }

	void build_vertex_buffer();
};

#endif