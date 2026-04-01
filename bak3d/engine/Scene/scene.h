/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <map>

#include "Core/global_definitions.h"
#include "Objects/axis.h"
#include "Objects/grid.h"
#include "Objects/light.h"
#include "Objects/particle_generator.h"

/*
 * This is the class that contains all the scene's data, such as references to cameras, lights, models, grids, etc.
 * Runs in the main loop of the project and is only one instance.
 */
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

	Camera* get_camera() const { return m_camera; }
private:
	static Scene* instance;
	
	void process_particle_activation();

	std::map<SceneObjectType, Object*> m_scene_objects;
	ParticleSystem* m_particle_system;
	Camera* m_camera;
	Light* m_light;
	Grid* m_grid;
	Axis* m_axis;
};

#endif