#include <algorithm>
#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "event_manager.h"

Camera::Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up, float speed, float hor_angle, float ver_angle, float zoom) :
	m_position(position),
	m_lookat(lookat),
	m_cam_up(up),
	m_cam_speed(speed),
	m_horizontal_angle(hor_angle),
	m_vertical_angle(ver_angle),
	m_zoom(zoom)
{
	std::cout << "Camera has been set up." << std::endl;
};

void Camera::update(float dt)
{
	// Prevent from having the camera move only when the cursor is within the windows
	EventManager::enable_mouse_cursor();

	// Mouse motion to get the variation in angle
	m_horizontal_angle -= EventManager::get_mouse_motion_x() * m_cam_speed * static_cast<double>(dt);
	m_vertical_angle -= EventManager::get_mouse_motion_y() * m_cam_speed * static_cast<double>(dt);

	// Clamp vertical angle to [-85, 85] degrees
	m_vertical_angle = std::max(-85.0f, std::min(85.0f, static_cast<float>(m_vertical_angle)));
	if (m_horizontal_angle > 360)
	{
		m_horizontal_angle -= 360;
	}
	else if (m_horizontal_angle < -360)
	{
		m_horizontal_angle += 360;
	}

	const auto theta = static_cast<float>(glm::radians(m_horizontal_angle));
	const auto phi = static_cast<float>(glm::radians(m_vertical_angle));

	// Set position of camera while orbiting lookat
	m_position = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
	// Set zoom based on camera scroll offset
	m_position *= EventManager::get_camera_scroll_offset();
}

glm::mat4 Camera::get_view_matrix() const
{
	return glm::lookAt(m_position, m_lookat, m_cam_up);
}

glm::mat4 Camera::get_projection_matrix() const
{
	return glm::perspective(glm::radians(m_zoom), static_cast<float>(EventManager::get_window_width()) / static_cast<float>(EventManager::get_window_height()), 0.1f, 100.0f);
}

glm::mat4 Camera::get_view_projection_matrix() const
{
	return get_projection_matrix() * get_view_matrix();
}