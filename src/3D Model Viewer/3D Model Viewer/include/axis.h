#ifndef AXIS_H
#define AXIS_H

#include <glad/glad.h> 

#include "camera.h"
#include "shader.h"

class Axis
{
public:
	Axis(Camera& camera);
	~Axis() = default;

	void draw() const;
private:
	Camera* m_camera;
	Shader* m_shader;
	GLuint m_vao{};
};

#endif