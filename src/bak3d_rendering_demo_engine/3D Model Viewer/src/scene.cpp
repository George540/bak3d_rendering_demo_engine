#include <iostream>
#include <glm/ext.hpp>
#include <filesystem>

#include "scene.h"
#include "renderer.h"
#include "model.h"
#include "file_loader.h"

using namespace std;

World* World::instance;

const auto model_path = std::filesystem::absolute("assets/backpack/backpack.obj").generic_string();

World::World()
{
	instance = this;

	// Camera Setup
	m_camera = new Camera(glm::vec3(10.0f, 5.0f, 10.0f), // position
						  glm::vec3(0.0f, 0.0f, 0.0f),   // lookat
						  glm::vec3(0.0f, 1.0f, 0.0f),   // up
						  10.0f,  // speed
						  315.0f, // horizontal angle
						  30.0f,  // vertical angle
						  45.0f); // zoom
	// Grid Setup
	m_grid = new Grid(*m_camera);
	m_axis = new Axis(*m_camera);

	// Object setup (will be later assigned during model selection process)
	m_model = nullptr;
	m_particle_system = new ParticleGenerator(std::filesystem::absolute("assets/particles-textures/default-particle.png").generic_string(), 2, *m_camera);

	// Light Setup
	m_light = new Light(glm::vec3(-3.0f, 3.0f, 3.0f), glm::vec3(0.1f, 0.1f, 0.1f), *m_camera);
	Renderer::environment_point_light = m_light;
}

World::~World()
{
	delete m_grid;
	delete m_camera;
	delete m_model;
	delete m_particle_system;
	delete m_light;
	delete m_axis;
}

void World::process_model_activation()
{
	// MODEL INSTANTIATION AND DELETION
	if (Renderer::object_current == 1 && Renderer::model_current != 0)
	{
		if (!m_model)
		{
			activate_current_model();
		}
		else if (m_model && Renderer::current_model_info.model_combo_index != m_model->m_combo_index)
		{
			replace_current_model();
		}
	}
	else if (Renderer::model_current == 0 && m_model)
	{
		deactivate_current_model();
	}
}

void World::activate_current_model()
{
	auto model_absolute_path = Renderer::current_model_info.model_file_path;
	m_model = new Model(model_absolute_path, *m_camera, *m_light, Renderer::current_model_info.model_combo_index);
	Renderer::current_model_info.current_model = m_model;
	cout << "Model " << Renderer::current_model_info.model_file_name << " has been activated." << endl;
}

void World::deactivate_current_model()
{
	Renderer::current_model_info.current_model = nullptr;
	delete m_model;
	m_model = nullptr;
	Renderer::current_model_info.set_info("None", "none", 0);
	cout << "Model " << Renderer::current_model_info.model_file_name << " has been deactivated." << endl;
}

void World::replace_current_model()
{
	cout << "Model " << m_model->m_name << " is being deactivated." << endl;
	Renderer::current_model_info.current_model = nullptr;
	delete m_model;
	m_model = nullptr;

	auto model_absolute_path = Renderer::current_model_info.model_file_path;
	m_model = new Model(model_absolute_path, *m_camera, *m_light, Renderer::current_model_info.model_combo_index);
	Renderer::current_model_info.current_model = m_model;
	
	cout << "Model " << Renderer::current_model_info.model_file_name << " has been activated." << endl;
}

void World::update(float dt) const
{
	m_camera->update(dt);
	m_light->update(dt);

	if (m_particle_system)
	{
		m_particle_system->update(dt, 2);
	}
}

void World::draw() const
{
	Renderer::begin_frame();

	if (Renderer::is_grid_rendering)
	{
		// Set depth test for axis to render in front of grid
		glDepthFunc(GL_ALWAYS);
		m_grid->draw();
		m_axis->draw();
		glDepthFunc(GL_LESS);
	}

	if (Renderer::is_full_render_selected)
	{
		m_light->draw();
	}

	if (m_model)
	{
		m_model->draw();
	}

	if (m_particle_system)
	{
		m_particle_system->draw();
	}

	Renderer::render_demo_window();
	//Renderer::post_processing();
	Renderer::end_frame();
}

void World::delete_arrays_and_buffers() const
{
	m_model->delete_mesh_vaos();
	m_grid->delete_vao_vbo();
	m_axis->delete_vao_vbo();
	m_light->delete_vao_vbo();
	m_particle_system->delete_vao_vbo();
}