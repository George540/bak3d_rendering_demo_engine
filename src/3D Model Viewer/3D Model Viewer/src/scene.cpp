#include <iostream>
#include <glm/ext.hpp>

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
	m_grid = new Grid();
	m_axis = new Axis(*m_camera);

	m_light = new Light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// Model set up
	const auto model_path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/backpack/backpack.obj";
	m_object = new Model(model_path, *m_camera);
	cout << "Model with path " << model_path << " has been spawned." << endl;
}

World::~World()
{
	delete m_grid;
	delete m_camera;
	delete m_object;
	delete m_light;
	delete m_axis;
}

void World::update(float dt) const
{
	// camera
	m_camera->update(dt);
    //cout << "\rCurrent frame dt: " << dt << ' ' << endl << flush;
}

void World::draw() const
{
    Renderer::begin_frame();

	m_grid->draw();
	m_axis->draw();
	m_object->draw();

	//m_light->Draw();

    Renderer::end_frame();
}
