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
GLuint Renderer::frame_buffer = NULL;
GLuint Renderer::texture_color_buffer = NULL;
GLuint Renderer::render_buffer = NULL;

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

	glfwSetFramebufferSizeCallback(r_window, EventManager::framebuffer_size_callback);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	cout << "Enabling depth test..." << endl;

	cout << "Ending Renderer Initialization..." << endl;
}

void Renderer::begin_frame()
{
	// Clear the screen
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Background Color
	glClearColor(UserInterface::background_color.r, UserInterface::background_color.g, UserInterface::background_color.b, 0.0f);
}

void Renderer::end_frame()
{
	// Swap buffers
	glDisable(GL_DEPTH_TEST);
	glfwSwapBuffers(r_window);
	glfwPollEvents();
}

void Renderer::shutdown()
{
	r_window = nullptr;
}