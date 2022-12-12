#include "light.h"
#include "shader.h"

#include <string>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <fstream>


Light::Light(glm::vec3 position, glm::vec3 scaling, Camera& camera) :
	mPosition(position),
	mScaling(scaling)
{
	mVAO = NULL;
	mVBO = NULL;

	BuildVertexBuffer();

	mCamera = &camera;
	mShader = new Shader(
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LightShader.vs",
		"D:/GitRepositories/3d_model_viewer_platform/Assets/Shaders/LightShader.fs");
	std::cout << "Light created..." << std::endl;
}

Light::~Light()
{
	delete mShader;
	delete mCamera;
}

void Light::Draw() const
{
	mShader->use();

	mShader->set_mat4("projection", mCamera->get_projection_matrix());
	mShader->set_mat4("view", mCamera->get_view_matrix());

	// World Matrix Update
	auto model = glm::mat4(1.0f);
	model = translate(model, mPosition) * scale(model, mScaling);
	mShader->set_mat4("model", model);

	// Draw the triangles !
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	glBindVertexArray(0);
}


void Light::BuildVertexBuffer()
{
	//cube model
	const float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindVertexArray(0);
}