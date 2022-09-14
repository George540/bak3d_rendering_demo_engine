#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

struct GLFWwindow;

class EventManager
{
	static GLFWwindow* m_window;
	static double last_frame_time;
	static float frame_time;
public:
	static void initialize();
	static void shutdown();

	static void update();

	static float get_frame_time();
	static bool is_exit_requested();

	static GLFWwindow* get_window();
};

#endif