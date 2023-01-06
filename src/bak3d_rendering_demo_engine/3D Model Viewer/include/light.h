// =====================================================================================
// Basic Light class that draws a cube light source
// Class made by George Mavroeidis, 2022
// =====================================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"

// Payload struct for light properties
struct light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float intensity;
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
	unsigned int m_vao;
	unsigned int m_vbo;
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
	[[nodiscard]] light get_light_properties() const { return m_properties; }
	void set_position(const glm::vec3& position) { m_position = position; }
	void set_scaling(const glm::vec3& scaling) { m_scaling = scaling; }

	[[nodiscard]] float get_horizontal_angle() const { return m_horizontal_angle; }
	void set_horizontal_angle(const float angle) { m_horizontal_angle = angle; }

	[[nodiscard]] float get_vertical_angle() const { return m_horizontal_angle; }
	void set_vertical_angle(const float angle) { m_vertical_angle = angle; }

	[[nodiscard]] float get_distance_offset() const { return m_distance_offset; }
	void set_distance_offset(const float distance) { m_distance_offset = distance; }

	void set_diffuse_color(const glm::vec3 col) { m_properties.diffuse = col; }
	void set_light_intensity(const float intensity) { m_properties.intensity = intensity; }

	[[nodiscard]] glm::vec3 get_position() const { return m_position; }
	[[nodiscard]] glm::vec3 get_scaling() const { return m_scaling; }

	void build_vertex_buffer();
	void delete_vao_vbo() const;
};

#endif