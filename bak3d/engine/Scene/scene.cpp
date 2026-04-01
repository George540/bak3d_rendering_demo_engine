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

#include <glm/glm/ext.hpp>
#include <filesystem>

#include "scene.h"

#include "editor.h"
#include "Objects/model.h"
#include "../Loader/file_loader.h"
#include "../Loader/resource_manager.h"
#include "../Renderer/material.h"
#include "../Renderer/renderer.h"
#include "../../Editor/user_interface.h"
#include "Core/global_settings.h"

using namespace std;

Scene* Scene::instance;

Scene::Scene()
{
	instance = this;

	// Unique material resources setup
	ResourceManager::add_material("grid", new Material(ResourceManager::get_shader("GridShader")));
	ResourceManager::add_material("line", new Material(ResourceManager::get_shader("LineShader")));
	ResourceManager::add_material("light", new Material(ResourceManager::get_shader("LightShader")));
	ResourceManager::add_material("particle", new Material(ResourceManager::get_shader("ParticleShader")));
	ResourceManager::add_material("bounding-box", new Material(ResourceManager::get_shader("GridShader")));

	// Camera Setup
	m_camera = new Camera(glm::vec3(10.0f, 5.0f, 10.0f), // position
						  glm::vec3(0.0f, 0.0f, 0.0f),   // lookat
						  glm::vec3(0.0f, 1.0f, 0.0f),   // up
						  10.0f,  // speed
						  315.0f, // horizontal angle
						  30.0f,  // vertical angle
						  45.0f); // zoom
	
	// Grid Setup
	m_grid = new Grid(ResourceManager::get_material("grid"));
	m_grid->set_camera(*m_camera);
	m_scene_objects[SceneObjectType::Grid] = m_grid;
	
	m_axis = new Axis(ResourceManager::get_material("line"));
	m_axis->set_camera(*m_camera);
	m_scene_objects[SceneObjectType::Axis] = m_axis;
	
	m_particle_system = new ParticleSystem(ResourceManager::get_material("particle"), ResourceManager::get_material("bounding-box"));
	m_particle_system->set_camera(*m_camera);
	UserInterface::current_particle_system = m_particle_system;
	m_scene_objects[SceneObjectType::ParticleSystem] = m_particle_system;

	// Light Setup
	m_light = new Light(glm::vec3(-3.0f, 3.0f, 3.0f), glm::vec3(0.1f, 0.1f, 0.1f), ResourceManager::get_material("light"));
	m_light->set_camera(*m_camera);
	UserInterface::environment_point_light = m_light;
	m_scene_objects[SceneObjectType::Light] = m_light;

	ResourceManager::set_camera(*m_camera, *m_light);
}

Scene::~Scene()
{
	delete m_grid;
	delete m_camera;
	delete m_particle_system;
	delete m_light;
	delete m_axis;
	m_scene_objects.clear();
}

void Scene::update(float dt) const
{
	m_camera->update(dt);
	if (GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::Light_Enabled))
	{
		m_light->update(dt);
	}
	m_particle_system->update(dt);
}

void Scene::draw() const
{
	Renderer::begin_frame();

	// Set depth test for axis to render in front of grid
	const bool is_grid_rendering = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::GridRendering);
	const bool is_axis_rendering = GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::AxisRendering);
	if (is_grid_rendering || is_axis_rendering)
	{
		glDepthFunc(GL_ALWAYS);
		if (is_grid_rendering)
		{
			m_grid->draw();
		}
		if (is_axis_rendering)
		{
			m_axis->draw();
		}
		glDepthFunc(GL_LESS);
	}

	if (GlobalSettings::get_global_setting_value<bool>(GlobalSettingOption::Light_Enabled))
	{
		m_light->draw();
	}
	/*if (!m_particle_system->is_visible() && UserInterface::is_full_render_selected)
	{
		m_light->draw();
	}*/

	/*if (auto current_model = ResourceManager::get_model(UserInterface::model_combo_items[UserInterface::model_current_index]))
	{
		current_model->draw();
	}*/

	if (m_particle_system && m_particle_system->is_visible())
	{
		//m_particle_system->draw();
	}

	Renderer::end_frame();

	Bak3DEditor::update();

	Renderer::post_end_frame();
}