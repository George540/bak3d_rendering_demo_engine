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

	static void enable_mouse_cursor();
	static void disable_mouse_cursor();
	static float get_random_float(float min, float max);
};

#endif