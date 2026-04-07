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

#include <iostream>

#include "light.h"

#include "Core/global_definitions.h"
#include "Input/event_manager.h"

#include <GLFW/glfw3.h>

#include "Asset/resource_manager.h"
#include "Asset/texture.h"
#include "Core/global_settings.h"
#include "Core/logger.h"

Light::Light(glm::vec3 position, glm::vec3 scaling, Material* material) :
	RenderableObject(material, position, "Light")
{
	m_position = position;
	m_scaling = scaling;

	m_vbo = new VertexBuffer(static_cast<GLsizei>(QUAD_VERTICES.size()) * VEC4_SIZE, QUAD_VERTICES.data());
	m_ebo = new ElementBuffer(static_cast<GLsizei>(QUAD_INDICES.size()) * UINT_SIZE, QUAD_INDICES.data());

	m_vao->set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, VEC4_SIZE, nullptr);
	m_vao->unbind_object();

	m_properties.position = m_position;
	m_properties.diffuse = glm::vec3(1.0f);
	m_properties.ambient = glm::vec3(0.3f);
	m_properties.specular = glm::vec3(0.3f);

	m_horizontal_angle = m_position.x;
	m_vertical_angle = m_position.y;
	m_distance_offset = glm::distance(m_position, glm::vec3(0.0f));

	m_sprite_texture = ResourceManager::get_texture("point_light_icon.png");

	B3D_LOG_INFO("Point light created.");
}

void Light::update(float dt)
{
	m_horizontal_angle = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_HorizontalRotation);
	m_vertical_angle = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_VerticalRotation);
	m_distance_offset = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_OriginDistance);
	m_properties.intensity = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Intensity);
	float sprite_scaling = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Scaling);
	
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_horizontal_angle += dt * 30.0f;
	}
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_horizontal_angle -= dt * 30.0f;
	}

	const auto theta = glm::radians(m_horizontal_angle);
	const auto phi = glm::radians(m_vertical_angle);
	m_position = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_distance_offset += dt * 30.0f;
	}
	if (glfwGetKey(EventManager::get_window(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_distance_offset -= dt * 30.0f;
	}
	m_position *= m_distance_offset;
	m_properties.position = m_position;

	m_scaling = glm::vec3(sprite_scaling, sprite_scaling, sprite_scaling);

	set_model_matrix(m_position, m_scaling, m_euler_rotation, 0.0f);

	const glm::vec4 diffuse = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::Light_Color);
	m_properties.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
}


void Light::draw() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	RenderableObject::draw();

	m_material->set_vec4("diffuseColor", glm::vec4(m_properties.diffuse, 1.0f));
	m_material->set_int("sprite", 0);

	m_sprite_texture->bind(0);

	m_vao->bind_object();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(CUBE_INDICES_SOLID.size()), GL_UNSIGNED_INT, nullptr);
	m_vao->unbind_object();

	Texture2D::unbind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
