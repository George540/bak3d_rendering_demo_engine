// =====================================================================================
// Camera class was made by George Mavroeidis using reference for the update function
// only from the COMP 371 lab 10
// =====================================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(glm::vec3 position,
		glm::vec3 lookat,
		glm::vec3 up, 
		float speed, 
		float hor_angle, 
		float ver_angle, 
		float zoom);
	~Camera() = default;

	void update(float dt);

	[[nodiscard]] glm::mat4 get_view_matrix() const;
	[[nodiscard]] glm::mat4 get_projection_matrix() const;
	[[nodiscard]] glm::mat4 get_view_projection_matrix() const;
	[[nodiscard]] glm::vec3 get_camera_position() const { return m_position; }
private:
	glm::vec3 m_position; // camera position
	glm::vec3 m_lookat; // look towards this point
	glm::vec3 m_cam_up; // up vector

	double m_cam_speed;
	double m_horizontal_angle; // horizontal angle
	double m_vertical_angle;   // vertical angle
	float m_zoom; // camera zoom
};

#endif