#include <iostream>
#include <glm/ext.hpp>
#include <filesystem>

#include "scene.h"
#include "renderer.h"
#include "model.h"

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

	// Light Setup
	m_light = new Light(glm::vec3(-3.0f, 3.0f, 3.0f), glm::vec3(0.1f, 0.1f, 0.1f), *m_camera);
	Renderer::environment_point_light = m_light;
}

World::~World()
{
	delete m_grid;
	delete m_camera;
	delete m_model;
	delete m_light;
	delete m_axis;
}

void World::process_model_activation()
{
	// MODEL INSTANTIATION AND DELETION
	if (Renderer::object_current == 1 && !m_model)
	{
		// Model set up
		m_model = new Model(model_path, *m_camera, *m_light);
		Renderer::current_model = m_model;
		cout << "Model with path " << model_path << " has been instantiated." << endl;
	}
	else if (Renderer::object_current != 1 && m_model)
	{
		Renderer::current_model = nullptr;
		delete m_model;
		m_model = nullptr;
		cout << "Model with path " << model_path << " has been deleted." << endl;
	}
}

void World::update(float dt) const
{
	// camera
	m_camera->update(dt);
	m_light->update(dt);
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
}