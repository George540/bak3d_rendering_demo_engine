// =====================================================================================
// Scene class was made by George Mavroeidis which is referenced from the COMP 371 lab 10.
// Mostly the structure of update and draw functions is inspired by the lab.
// =====================================================================================

#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "Objects/axis.h"
#include "Objects/grid.h"
#include "Objects/light.h"
#include "Objects/particle_generator.h"

class Scene
{
public:
	Scene();
	~Scene();

	static Scene* get_instance()
	{
		static Scene* instance;
		return instance;
	}

	void update(float dt) const;
	void draw() const;
private:
	static Scene* instance;
	
	void process_particle_activation();
	
	ParticleSystem* m_particle_system;
	Camera* m_camera;
	Light* m_light;
	Grid* m_grid;
	Axis* m_axis;
};

#endif