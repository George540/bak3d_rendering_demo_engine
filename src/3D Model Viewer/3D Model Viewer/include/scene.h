#ifndef SCENE_H
#define SCENE_H

#include <glm/fwd.hpp>
#include <glad.h>

#include "model.h"
#include "camera.h"
#include "grid.h"
#include "axis.h"
#include "light.h"

class World
{
public:
	World();
	~World();

	static World* get_instance()
	{
		static World* instance;
		return instance;
	}

	void update(float dt) const;
	void draw() const;
	Model* get_model() const { return m_model; }
private:
	static World* instance;

	Model* m_model;
	Camera* m_camera;
	Light* m_light;
	Grid* m_grid;
	Axis* m_axis;
};

#endif