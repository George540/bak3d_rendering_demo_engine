#include "event_manager.h"
//#include "renderer.h"

#include <iostream>
#include <GLFW/glfw3.h>

using namespace std;

// Time
double EventManager::last_frame_time = glfwGetTime();
float EventManager::frame_time = 0.0f;

// Mouse
double EventManager::mouse_pos_x = 0.0;
double EventManager::mouse_pos_y = 0.0;
double EventManager::last_mouse_position_x = 0.0;
double EventManager::last_mouse_position_y = 0.0;
double EventManager::delta_x = 0.0;
double EventManager::delta_y = 0.0;
double EventManager::cam_zoom_distance = 10.0;
double EventManager::cam_zoom_factor = 1.0;
int EventManager::last_mouse_left_state = GLFW_RELEASE;
int EventManager::last_mouse_right_state = GLFW_RELEASE;
int EventManager::last_mouse_middle_state = GLFW_RELEASE;
bool EventManager::is_using_diffuse_texture = true;
bool EventManager::is_using_specular_texture = true;
bool EventManager::is_using_normal_maps = true;

GLFWwindow* EventManager::m_window = nullptr;

void EventManager::initialize()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW.\n";
	}
	else
	{
		std::cout << "Initializing GLFW..." << endl;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

#if defined(PLATFORM_OSX)
	// OS X would use legacy opengl by default, and wouldn't be able to load shaders
	// This is the proper way to setup GLFW to use modern OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);

#else
	// Allow older laptops to run the framework, even though, our framework
	// is compatible with OpenGL 3.3 (which removed many deprecated features)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
#endif

	// Open a window and create its OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, 1);
	m_window = glfwCreateWindow(1920, 1080, "3D Model Viewer (WIP)", nullptr, nullptr);

	if (m_window == nullptr)
	{
		cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n";
		glfwTerminate();
		exit(-1);  // NOLINT(concurrency-mt-unsafe)
	}
	else
	{
		std::cout << "Opening Window..." << endl;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initial time
	last_frame_time = glfwGetTime();
	srand(static_cast<unsigned int>(time(nullptr)));  // NOLINT(cert-msc51-cpp)
	std::cout << "Ending Event Manager Initialization..." << endl;
}

void EventManager::shutdown()
{
	// Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	glfwTerminate();
	m_window = nullptr;
	std::cout << "Terminating EventManager..." << endl;
}

void EventManager::update()
{
	// Update inputs / events
	glfwPollEvents();

	// Update mouse positions
	glfwGetCursorPos(m_window, &mouse_pos_x, &mouse_pos_y);
	// Camera tilt and Pan
	if (last_mouse_right_state == GLFW_RELEASE && glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		delta_x = static_cast<float>(mouse_pos_x - last_mouse_position_x);  // NOLINT(clang-diagnostic-double-promotion)
		delta_y = -static_cast<float>(mouse_pos_y - last_mouse_position_y);  // NOLINT(clang-diagnostic-double-promotion)
	}
	else
	{
		delta_x = 0.0;
		delta_y = 0.0;
	}
	last_mouse_position_x = mouse_pos_x;
	last_mouse_position_y = mouse_pos_y;

	// Update mouse zoom via scroll wheel
	glfwSetScrollCallback(m_window, scroll_callback);
	// Toggle diffuse texture usage
	glfwSetKeyCallback(m_window, toggle_diffuse_callback);

	// Update frame time
	const double current_time = glfwGetTime();
	frame_time = static_cast<float>(current_time - last_frame_time);
	last_frame_time = current_time;
}

float EventManager::get_frame_time()
{
	return frame_time;
}

bool EventManager::is_exit_requested()
{
	return glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(m_window);
}

GLFWwindow* EventManager::get_window()
{
	return m_window;
}

double EventManager::get_mouse_motion_x()
{
	return delta_x;
}

double EventManager::get_mouse_motion_y()
{
	return delta_y;
}

double EventManager::get_camera_scroll_offset()
{
	return cam_zoom_distance;
}


void EventManager::enable_mouse_cursor()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void EventManager::disable_mouse_cursor()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

float EventManager::get_random_float(float min, float max)
{
	const auto value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  // NOLINT(concurrency-mt-unsafe)

	return min + value * (max - min);
}

void EventManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	cam_zoom_distance -= yoffset;
	// Clamp zoom to [1, 10] degrees
	cam_zoom_distance = std::max(1.0, std::min(25.0, cam_zoom_distance));
}

void EventManager::toggle_diffuse_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		is_using_diffuse_texture = !is_using_diffuse_texture;
	}
}
