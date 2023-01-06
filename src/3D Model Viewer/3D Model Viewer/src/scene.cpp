#include <iostream>
#include <glm/ext.hpp>
#include <filesystem>

#include "scene.h"
#include "renderer.h"
#include "model.h"

using namespace std;

World* World::instance;

World::World()
{
	instance = this;

	// Camera Setup
	m_camera = new Camera(glm::vec3(10.0f, 5.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 315.0f, 30.0f, 45.0f);
	// Grid Setup
	m_grid = new Grid(*m_camera);
	m_axis = new Axis(*m_camera);

	m_light = new Light(glm::vec3(-3.0f, 3.0f, 3.0f), glm::vec3(0.1f, 0.1f, 0.1f), *m_camera);

	// Model set up
	const auto model_path = std::filesystem::absolute("assets/backpack/backpack.obj").generic_string();
	m_model = new Model(model_path, *m_camera, *m_light);
	Renderer::current_model = m_model;
	Renderer::environment_point_light = m_light;
	cout << "Model with path " << model_path << " has been spawned." << endl;
}

World::~World()
{
	delete m_grid;
	delete m_camera;
	delete m_model;
	delete m_light;
	delete m_axis;
}

void World::update(float dt) const
{
	// camera
	m_camera->update(dt);
	m_light->update(dt);
    //cout << "\rCurrent frame dt: " << dt << ' ' << endl << flush;
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

	m_model->draw();

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