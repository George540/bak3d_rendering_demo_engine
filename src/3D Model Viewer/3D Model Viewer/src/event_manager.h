#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <GLFW/glfw3.h>

struct GLFWwindow;

class EventManager
{
	// Window
	static GLFWwindow* m_window;

	// Time
	static double last_frame_time;
	static float frame_time;

	// Mouse
	static double mouse_pos_x;
	static double mouse_pos_y;
	static double last_mouse_position_x;
	static double last_mouse_position_y;
	static double delta_x;
	static double delta_y;
	static double cam_zoom_distance;
	static double cam_zoom_factor;
	static int last_mouse_left_state;
	static int last_mouse_right_state;
	static int last_mouse_middle_state;
public:
	static void initialize();
	static void shutdown();

	static void update();

	static float get_frame_time();
	static bool is_exit_requested();

	static GLFWwindow* get_window();

	static double get_mouse_motion_x();
	static double get_mouse_motion_y();
	static double get_camera_scroll_offset();

	static bool get_using_diffuse_texture() { return is_using_diffuse_texture; }
	static void set_using_diffuse_texture(bool isDiffuse) { is_using_diffuse_texture = isDiffuse; }

	static bool get_using_specular_texture() { return is_using_specular_texture; }
	static void set_using_specular_texture(bool isSpecular) { is_using_specular_texture = isSpecular; }

	static bool get_using_normal_maps() { return is_using_normal_maps; }
	static void set_using_normal_maps(bool isNormal) { is_using_normal_maps = isNormal; }

	static void enable_mouse_cursor();
	static void disable_mouse_cursor();
	static float get_random_float(float min, float max);

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void toggle_diffuse_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static bool is_using_diffuse_texture;
	static bool is_using_specular_texture;
	static bool is_using_normal_maps;
};

#endif