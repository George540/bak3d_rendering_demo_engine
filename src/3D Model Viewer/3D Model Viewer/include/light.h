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
	float m_horizontal_angle;
	float m_vertical_angle;
	float m_distance_offset;

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

	void update(float dt);
	void draw() const;
	light get_light_properties() const { return m_properties; }
	void set_position(const glm::vec3& position) { m_position = position; }
	void set_scaling(const glm::vec3& scaling) { m_scaling = scaling; }

	float get_horizontal_angle() const { return m_horizontal_angle; }
	void set_horizontal_angle(const float angle) { m_horizontal_angle = angle; }

	float get_vertical_angle() const { return m_horizontal_angle; }
	void set_vertical_angle(const float angle) { m_vertical_angle = angle; }

	float get_distance_offset() const { return m_distance_offset; }
	void set_distance_offset(const float distance) { m_distance_offset = distance; }

	glm::vec3 get_position() const { return m_position; }
	glm::vec3 get_scaling() const { return m_scaling; }

	void build_vertex_buffer();
};

#endif