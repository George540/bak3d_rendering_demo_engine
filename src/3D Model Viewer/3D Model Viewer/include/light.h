#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <vector>
#include <glad.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

class Light
{
	// ORIENTATION
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::mat4 mWorldTransformationMatrix;

	unsigned int mVAO;
	unsigned int mVBO;
	std::vector<glm::vec3> mVertexPositions;
	GLuint mShader;

public:
	Light(glm::vec3 position, glm::vec3 scale);
	~Light();

	void Draw();
	void SetPosition(glm::vec3& position) { mPosition = position; }
	void SetScaling(glm::vec3& scaling) { mScaling = scaling; }
	glm::vec3 GetPosition() const { return mPosition; }
	glm::vec3 GetScaling() const { return mScaling; }
	glm::mat4 GetWorldTransformationMatrix() const { return mWorldTransformationMatrix; }

	unsigned int GetVAO() { return mVAO; }

	void BuildVertexBuffer();

	GLuint LoadShader(std::string vertex_shader_path, std::string fragment_shader_path);
};

#endif