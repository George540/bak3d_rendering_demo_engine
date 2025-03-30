// =====================================================================================
// Scene class was made by George Mavroeidis which is referenced from the COMP 371 lab 10.
// Mostly the structure of update and draw functions is inspired by the lab.
// =====================================================================================

#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <glm/fwd.hpp>
#include <glad.h>
#include <unordered_map>

#include "model.h"
#include "camera.h"
#include "grid.h"
#include "axis.h"
#include "light.h"
#include "particle_generator.h"

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