#include <iostream>
#include <filesystem>

#include "renderer.h"
#include "event_manager.h"
#include "user_interface.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "stb_image.h"


using namespace std;

// Renderer
GLFWwindow* Renderer::r_window = nullptr;
FrameBuffer* Renderer::frame_buffer = nullptr;

void Renderer::initialize()
{
	r_window = EventManager::get_window();
	glfwMakeContextCurrent(r_window);
	cout << "Capturing window at Renderer..." << endl;

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		cerr << "Failed to initialize GLAD" << endl;
		exit(-1);
	}
	cout << "Initializing GLAD..." << endl;

	glfwSetFramebufferSizeCallback(r_window, on_framebuffer_size_callback);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	cout << "Enabling depth test..." << endl;

	frame_buffer = new FrameBuffer(0, nullptr, EventManager::get_window_width(), EventManager::get_window_height(), GL_NONE);

	cout << "Ending Renderer Initialization..." << endl;
}

void Renderer::begin_frame()
{
	// Clear the screen
	frame_buffer->bind_object();
	glClearColor(UserInterface::background_color.r, UserInterface::background_color.g, UserInterface::background_color.b, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end_frame()
{
	
	// Swap buffers
	glDisable(GL_DEPTH_TEST);
	frame_buffer->unbind_object();
	glClearColor(UserInterface::background_color.r, UserInterface::background_color.g, UserInterface::background_color.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::post_end_frame()
{
	glfwSwapBuffers(r_window);
	glfwPollEvents();
}

void Renderer::shutdown()
{
	r_window = nullptr;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
/**
 * \brief Whenever the window size changed (by OS or user resize) this callback function executes
 * \param window The OpenGL main window
 * \param newWidth The OpenGL main window width
 * \param newHeight The OpenGL main window height
 */
void Renderer::on_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	// Note: position is set to 0 for both x and y coordinates
	frame_buffer->resize(newWidth, newHeight);
	glViewport(0, 0, newWidth, newHeight);
}