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
	m_camera = new Camera(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, -30.0f, 45.0f);

	// Object set up
	const auto model_path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/backpack/backpack.obj";
	m_object = new Model(model_path);
	//m_object = new Object();

	m_light = new Light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

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
	delete m_light;
}

void World::update(float dt)
{
	// camera
	m_camera->update(dt);
    //cout << "\rCurrent frame dt: " << dt << ' ' << endl << flush;
}

void World::draw()
{
    Renderer::begin_frame();

	ourShader->use();

	ourShader->set_mat4("projection", m_camera->get_projection_matrix());
	ourShader->set_mat4("view", m_camera->get_view_matrix());

	glm::mat4 model = glm::mat4(1.0f);
	ourShader->set_mat4("model", model);

	m_light->Draw();

	m_object->draw(*ourShader);

    Renderer::end_frame();
}