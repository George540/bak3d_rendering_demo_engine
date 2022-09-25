#ifndef SCENE_H
#define SCENE_H

#include <glm/fwd.hpp>
#include <glad.h>

#include "model.h"

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

	void generate_grid();
	void update(float dt);
	void draw();
	void set_matrix(GLuint shader_program, glm::mat4 world_matrix);

	void set_projection_matrix(int shader_program, glm::mat4 projection_matrix);
	void set_view_matrix(int shader_program, glm::mat4 view_matrix);
	void set_world_matrix(int shader_program, glm::mat4 world_matrix);
private:
	static World* instance;

	Object* m_object;
	//Camera* m_Camera;
};

#endif