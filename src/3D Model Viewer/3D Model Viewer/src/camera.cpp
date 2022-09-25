#include "camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up, float speed, float hor_angle, float ver_angle, float zoom) :
	m_position(position),
	m_lookat(lookat),
	m_cam_up(up),
	m_cam_speed(speed),
	m_horizontal_angle(hor_angle),
	m_vertical_angle(ver_angle),
	m_zoom(zoom)
{

};

void Camera::update(float dt)
{
	
}

glm::mat4 Camera::get_view_matrix() const
{
	return glm::lookAt(m_position, m_lookat, m_cam_up);
}

glm::mat4 Camera::get_projection_matrix() const
{
	return glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, -0.1f, 100.0f);
}

glm::mat4 Camera::get_view_projection_matrix() const
{
	return get_projection_matrix() * get_view_matrix();
}
