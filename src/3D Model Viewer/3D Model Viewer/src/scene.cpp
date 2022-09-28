#include <vector>
#include <iostream>

#include "scene.h"
#include "renderer.h"
#include "model.h"


using namespace std;

World* World::instance;
Shader* ourShader;

World::World()
{
	instance = this;

	// Camera Setup
	m_camera = new Camera(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 90.0f, -30.0f, 45.0f);

	// Object set up
	const auto model_path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/backpack/backpack.obj";
	//m_object = new Model(FileSystem::getPath(path));
	//m_object = new Object();

	// Model Set up
	string shader_path = "D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/";
	ourShader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/1.model_loading.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/1.model_loading.fs");
}

World::~World()
{
	// delete camera
	delete m_camera;
	delete m_object;
}

void World::update(float dt)
{
	// camera
    cout << "\rCurrent frame dt: " << dt << ' ' << endl << flush;
}

void World::draw()
{
    Renderer::begin_frame();

	ourShader->use();
	cout << "Using shader" << endl;

	ourShader->set_mat4("projection", m_camera->get_projection_matrix());
	ourShader->set_mat4("view", m_camera->get_view_matrix());

	glm::mat4 model = glm::mat4(1.0f);
	ourShader->set_mat4("model", model);
	cout << "Set up shader" << endl;

	m_object->draw(*ourShader);
	cout << "Draw object" << endl;

    Renderer::end_frame();
}