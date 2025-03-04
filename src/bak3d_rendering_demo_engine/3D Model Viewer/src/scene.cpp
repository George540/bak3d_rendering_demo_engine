#include <iostream>
#include <glm/ext.hpp>
#include <filesystem>

#include "scene.h"
#include "renderer.h"
#include "model.h"
#include "file_loader.h"
#include "resource_manager.h"
#include "user_interface.h"

using namespace std;

Scene* Scene::instance;

const auto model_path = std::filesystem::absolute("assets/backpack/backpack.obj").generic_string();

Scene::Scene()
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
	m_grid = new Grid(*m_camera, *ResourceManager::get_shader("GridShader"));
	m_axis = new Axis(*m_camera, *ResourceManager::get_shader("LineShader"));

	// Object setup (will be later assigned during model selection process)
	m_model = nullptr;
	m_particle_system = nullptr;

	// Light Setup
	m_light = new Light(glm::vec3(-3.0f, 3.0f, 3.0f), glm::vec3(0.1f, 0.1f, 0.1f), *m_camera, *ResourceManager::get_shader("LightShader"));
	UserInterface::environment_point_light = m_light;
}

Scene::~Scene()
{
	delete m_grid;
	delete m_camera;
	delete m_model;
	delete m_particle_system;
	delete m_light;
	delete m_axis;
}

void Scene::process_object_activation()
{
	process_model_activation();
	process_particle_activation();
}

void Scene::process_model_activation()
{
	// MODEL INSTANTIATION AND DELETION
	if (UserInterface::object_current == 1 && UserInterface::model_current != 0)
	{
		if (!m_model)
		{
			activate_current_model();
		}
		else if (m_model && UserInterface::current_model_info.model_combo_index != m_model->m_combo_index)
		{
			replace_current_model();
		}
	}
	else if ((UserInterface::model_current == 0 && m_model) || (m_model && UserInterface::object_current == 0))
	{
		deactivate_current_model();
	}
}

void Scene::activate_current_model()
{
	auto model_absolute_path = UserInterface::current_model_info.model_file_path;
	m_model = new Model(model_absolute_path, *m_camera, *m_light, UserInterface::current_model_info.model_combo_index);
	UserInterface::current_model_info.current_model = m_model;

	cout << "Model " << UserInterface::current_model_info.model_file_name << " has been activated." << endl;
}

void Scene::deactivate_current_model()
{
	UserInterface::current_model_info.current_model = nullptr;
	delete m_model;
	m_model = nullptr;
	UserInterface::current_model_info.set_info("None", "none", 0);

	cout << "Model " << UserInterface::current_model_info.model_file_name << " has been deactivated." << endl;
}

void Scene::replace_current_model()
{
	cout << "Model " << m_model->m_file_name << " is being deactivated." << endl;
	UserInterface::current_model_info.current_model = nullptr;
	delete m_model;
	m_model = nullptr;

	auto model_absolute_path = UserInterface::current_model_info.model_file_path;
	m_model = new Model(model_absolute_path, *m_camera, *m_light, UserInterface::current_model_info.model_combo_index);
	UserInterface::current_model_info.current_model = m_model;
	
	cout << "Model " << UserInterface::current_model_info.model_file_name << " has been activated." << endl;
}

void Scene::process_particle_activation()
{
	if (UserInterface::object_current == 2 && !UserInterface::current_particle_system)
	{
		m_particle_system = new ParticleSystem(*m_camera);
		UserInterface::current_particle_system = m_particle_system;

		cout << "Particle System has been activated." << endl;
	}
	else if (UserInterface::current_particle_system && UserInterface::object_current != 2)
	{
		UserInterface::particle_payload_info = particle_info();
		UserInterface::current_particle_system = nullptr;
		delete m_particle_system;
		m_particle_system = nullptr;

		cout << "Particle System has been deactivated." << endl;
	}
	else if (UserInterface::object_current == 2 && m_particle_system && m_particle_system->get_particle_amount() != UserInterface::particle_payload_info.amount)
	{
		UserInterface::current_particle_system = nullptr;
		delete m_particle_system;
		m_particle_system = nullptr;

		m_particle_system = new ParticleSystem(*m_camera, UserInterface::particle_payload_info);
		UserInterface::current_particle_system = m_particle_system;
	}
}

void Scene::update(float dt) const
{
	m_camera->update(dt);
	m_light->update(dt);

	if (m_particle_system)
	{
		m_particle_system->update(dt, 2);
	}
}

void Scene::draw() const
{
	Renderer::begin_frame();
	UserInterface::begin_frame();

	UserInterface::render_demo_window();

	if (UserInterface::is_grid_rendering)
	{
		// Set depth test for axis to render in front of grid
		glDepthFunc(GL_ALWAYS);
		m_grid->draw();
		m_axis->draw();
		glDepthFunc(GL_LESS);
	}

	if (!UserInterface::current_particle_system && UserInterface::is_full_render_selected)
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

	UserInterface::end_frame();
	//Renderer::post_processing();
	Renderer::end_frame();
}

void Scene::delete_arrays_and_buffers() const
{
	m_particle_system->delete_vao_vbo();
}