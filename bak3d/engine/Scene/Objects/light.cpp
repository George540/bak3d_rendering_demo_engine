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

	m_light_data_ubo = std::make_unique<UniformBuffer>(7 * VEC4_SIZE /*Temporary size*/, nullptr, 1, GL_DYNAMIC_DRAW);

	m_diffuse = glm::vec3(1.0f);
	m_ambient = glm::vec3(0.3f);
	m_specular = glm::vec3(0.3f);

	m_constant = 1.0f;
	m_linear = 0.09f;
	m_quadratic = 0.032f;

	m_inner_cut_off = glm::cos(glm::radians(12.5f));
	m_outer_cut_off = glm::cos(glm::radians(17.5f));

	m_horizontal_angle = m_position.x;
	m_vertical_angle = m_position.y;
	m_distance_offset = glm::distance(m_position, glm::vec3(0.0f));

	B3D_LOG_INFO("Light created.");
}

void Light::update(float dt)
{
	m_horizontal_angle = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_HorizontalRotation);
	m_vertical_angle = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_VerticalRotation);
	m_distance_offset = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_OriginDistance);
	m_intensity = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Intensity);
	const float sprite_scaling = GlobalSettings::get_global_setting_value<float>(GlobalSettingOption::Light_Scaling);
	
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

	m_scaling = glm::vec3(sprite_scaling, sprite_scaling, sprite_scaling);

	set_model_matrix(m_position, m_scaling, m_euler_rotation, 0.0f);

	const glm::vec4 diffuse = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::Light_Color);
	m_diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

	const LightType type = static_cast<LightType>(GlobalSettings::get_global_setting_value<uint32_t>(GlobalSettingOption::Light_Type));
	set_texture_by_type(type);

	m_direction = glm::normalize(m_position - glm::vec3(0.0f));

	update_light_data_ubo();
}

void Light::draw() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	RenderableObject::draw();

	m_material->set_vec4("diffuseColor", glm::vec4(m_diffuse, 1.0f));
	m_material->set_int("sprite", 0);

	m_sprite_texture->bind(0);

	m_vao->bind_object();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(QUAD_INDICES.size()), GL_UNSIGNED_INT, nullptr);
	m_vao->unbind_object();

	Texture2D::unbind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Light::set_attenuation(const float constant, const float linear, const float quadratic)
{
	m_constant = constant;
	m_linear = linear;
	m_quadratic = quadratic;
}

void Light::set_cone_angles(const float inner_degrees, const float outer_degrees)
{
	m_inner_cut_off = glm::cos(glm::radians(inner_degrees));
	m_outer_cut_off = glm::cos(glm::radians(outer_degrees));
}

void Light::update_light_data_ubo() const
{
	m_light_data_ubo->bind_object();
	// vec4 position
	m_light_data_ubo->set_buffer_sub_data(&m_position,        VEC3_SIZE,  0 * VEC4_SIZE + 0);
	m_light_data_ubo->set_buffer_sub_data(&m_inner_cut_off,         FLOAT_SIZE, 0 * VEC4_SIZE + VEC3_SIZE);

	// vec4 direction
	m_light_data_ubo->set_buffer_sub_data(&m_direction,       VEC3_SIZE,  1 * VEC4_SIZE + 0);
	m_light_data_ubo->set_buffer_sub_data(&m_outer_cut_off,   FLOAT_SIZE, 1 * VEC4_SIZE + VEC3_SIZE);

	// vec4 ambient
	m_light_data_ubo->set_buffer_sub_data(&m_ambient,         VEC3_SIZE,  2 * VEC4_SIZE + 0);
	m_light_data_ubo->set_buffer_sub_data(&m_constant,        FLOAT_SIZE, 2 * VEC4_SIZE + VEC3_SIZE);

	// vec4 diffuse
	m_light_data_ubo->set_buffer_sub_data(&m_diffuse,         VEC3_SIZE,  3 * VEC4_SIZE + 0);
	m_light_data_ubo->set_buffer_sub_data(&m_linear,          FLOAT_SIZE, 3 * VEC4_SIZE + VEC3_SIZE);

	// vec4 specular
	m_light_data_ubo->set_buffer_sub_data(&m_specular,        VEC3_SIZE,  4 * VEC4_SIZE + 0);
	m_light_data_ubo->set_buffer_sub_data(&m_quadratic,       FLOAT_SIZE, 4 * VEC4_SIZE + VEC3_SIZE);

	// vec4 intensity (.a only, .rgb is padding)
	m_light_data_ubo->set_buffer_sub_data(&m_intensity,       FLOAT_SIZE, 5 * VEC4_SIZE + VEC3_SIZE);

	// int type + float padding[3]
	const int32_t type = static_cast<int32_t>(m_type);
	m_light_data_ubo->set_buffer_sub_data(&type,              INT_SIZE,   6 * VEC4_SIZE);

	m_light_data_ubo->unbind_object();
}

void Light::set_texture_by_type(const LightType type)
{
	if (m_type == type)
	{
		return;
	}

	m_type = type;
	switch (m_type)
	{
		case LightType::Directional: m_sprite_texture = ResourceManager::get_texture("directional_light_icon.png"); break;
		case LightType::Point: m_sprite_texture = ResourceManager::get_texture("point_light_icon.png"); break;
		case LightType::Spot: m_sprite_texture = ResourceManager::get_texture("spot_light_icon.png"); break;
		case LightType::Area: m_sprite_texture = ResourceManager::get_texture("area_light_icon.png"); break;
		default: m_sprite_texture = ResourceManager::get_texture("point_light_icon.png"); break;
	}
}
