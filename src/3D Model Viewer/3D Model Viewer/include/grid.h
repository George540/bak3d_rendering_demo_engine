#ifndef GRID_H
#define GRID_H

#include <glad/glad.h>

#include "shader.h"
#include "camera.h"

class Grid
{
public:
	Grid(Camera& cam);
	~Grid() = default;

	void draw() const;
	void delete_vao_vbo() const;
private:
	GLuint m_vao{};
	GLuint m_vbo{};
	Shader* m_shader;
	Camera* m_camera;
	int number_of_elements;
	int number_of_slices;
	float grid_size;
};

#endif