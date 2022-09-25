#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up);

	void update(float dt);

	glm::mat4 get_view_matrix() const;
	glm::mat4 get_projection_matrix() const;
	glm::mat4 get_view_projection_matrix() const;
private:
	glm::vec3 m_position;
	glm::vec3 m_lookat;
	glm::vec3 m_cam_up;
	float m_cam_speed;
	float zoom;
};

#endif