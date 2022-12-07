#include <iostream>

#include "renderer.h"
#include "event_manager.h"

#include <GLFW/glfw3.h>

#include "stb_image.h"


using namespace std;

GLFWwindow* Renderer::r_window = nullptr;

void Renderer::initialize()
{
	r_window = EventManager::get_window();
	glfwMakeContextCurrent(r_window);
	std::cout << "Capturing window at Renderer..." << endl;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }    

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Gray background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	std::cout << "Setting background color..." << endl;

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	std::cout << "Enabling depth test..." << endl;

	//check_for_errors();
	std::cout << "Ending Renderer Initialization..." << endl;
}

void Renderer::shutdown()
{
	// Managed by EventManager
	r_window = nullptr;
}

void Renderer::begin_frame()
{
	// Clear the screen
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end_frame()
{
	// Swap buffers
	glDisable(GL_DEPTH_TEST);
	glfwSwapBuffers(r_window);
	glfwPollEvents();
}