// =====================================================================================
// Basic axis class that draws the XYZ 3D axis lines
// Class made by George Mavroeidis, 2022
// =====================================================================================

#ifndef AXIS_H
#define AXIS_H

#include <glad/glad.h> 

#include "camera.h"
#include "shader.h"

class Axis
{
public:
	Axis(Camera& camera);
	~Axis();

	void draw() const;
	void delete_vao_vbo() const;
private:
	Camera* m_camera;
	Shader* m_shader;
	GLuint m_vao{};
	GLuint m_vbo{};
};

#endif