/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#pragma once

#include "renderable_object.h"

// Payload struct for light properties
struct light
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float intensity;
};

/*
 * Basic Light class that draws a cube light source (will use sprite instead of cube in the future).
 */
class Light : public RenderableObject
{
	// ORIENTATION
	float m_horizontal_angle;
	float m_vertical_angle;
	float m_distance_offset;

	// PROPERTIES
	light m_properties{};

public:
	Light(glm::vec3 position, glm::vec3 scaling, Material* material);
	~Light() override = default;

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
};