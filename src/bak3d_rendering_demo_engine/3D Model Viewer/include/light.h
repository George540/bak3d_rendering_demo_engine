// =====================================================================================
// Basic Light class that draws a cube light source
// Class made by George Mavroeidis, 2022
// =====================================================================================

#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"

// Payload struct for light properties
struct light
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float intensity;
};

class Light : public Object
{
	// ORIENTATION
	float m_horizontal_angle;
	float m_vertical_angle;
	float m_distance_offset;

	// PROPERTIES
	light m_properties{};

public:
	Light(glm::vec3 position, glm::vec3 scaling, Camera& camera, Shader& shader);
	~Light() override {};

	void update(float dt) override;
	void draw() const override;

	[[nodiscard]] light get_light_properties() const { return m_properties; }

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
};

#endif