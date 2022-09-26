#include <vector>
#include <iostream>

#include "scene.h"
#include "renderer.h"
#include "object.h"
#include <glm/gtc/type_ptr.hpp>


using namespace std;

World* World::instance;

World::World()
{
	instance = this;

	// Camera Setup

	// Object set up
	auto path = "D:/GitRepositories/3d_model_viewer_platform/src/3D Model Viewer/3D Model Viewer/assets/untitled.obj";
	//m_object = new Model(FileSystem::getPath(path));
	m_object = new Object();
}

World::~World()
{
	// delete camera
}

void World::update(float dt)
{
	// camera
    cout << "\rCurrent frame dt: " << dt << ' ' << flush;
}

void World::draw()
{
    Renderer::begin_frame();
	m_object->draw();
    Renderer::end_frame();
}