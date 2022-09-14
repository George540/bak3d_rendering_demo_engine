#include "event_manager.h"
#include "renderer.h"

#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

double EventManager::last_frame_time = glfwGetTime();
float EventManager::frame_time = 0.0f;

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
	srand(static_cast<unsigned int>(time(nullptr)));
	std::cout << "Ending Event Manager Initialization..." << endl;
}

void EventManager::shutdown()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	m_window = nullptr;
	std::cout << "Terminating EventManager..." << endl;
}

void EventManager::update()
{
	// Update frame time
	double current_time = glfwGetTime();
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