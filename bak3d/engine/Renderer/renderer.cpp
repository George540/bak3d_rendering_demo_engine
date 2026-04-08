/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#include <iostream>
#include <filesystem>

#include "renderer.h"

#include "Core/global_settings.h"
#include "Input/event_manager.h"

#include "imgui.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Scene/scene.h"

using namespace std;

// Renderer
GLFWwindow* Renderer::r_window = nullptr;
FrameBuffer* Renderer::frame_buffer = nullptr;
UniformBuffer* Renderer::matrix_uniform_buffer = nullptr;

void Renderer::initialize()
{
	r_window = EventManager::get_window();
	glfwMakeContextCurrent(r_window);
	B3D_LOG_INFO("Capturing window at Renderer...");

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		B3D_LOG_ERROR("Failed to initialize GLAD");
		exit(-1);
	}
	B3D_LOG_INFO("Initializing GLAD context...");

	glfwSetFramebufferSizeCallback(r_window, on_framebuffer_size_callback);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	B3D_LOG_INFO("Enabling depth test...");

	frame_buffer = new FrameBuffer(0, nullptr, EventManager::get_window_width(), EventManager::get_window_height(), GL_NONE);
	matrix_uniform_buffer = new UniformBuffer(MAT4_SIZE * 2, nullptr, 0, GL_DYNAMIC_DRAW);

	B3D_LOG_INFO("Ending Renderer Initialization....");
}

void Renderer::begin_frame()
{
	glm::mat4 projection_matrix = Scene::instance->get_camera()->get_projection_matrix();
	glm::mat4 view_matrix = Scene::instance->get_camera()->get_view_matrix();

	matrix_uniform_buffer->bind_object();
	matrix_uniform_buffer->set_buffer_sub_data(glm::value_ptr(projection_matrix), MAT4_SIZE, 0);
	matrix_uniform_buffer->set_buffer_sub_data(glm::value_ptr(view_matrix), MAT4_SIZE, MAT4_SIZE);
	matrix_uniform_buffer->unbind_object();
	
	// Clear the screen
	frame_buffer->bind_object();

	const auto background_color = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::BackgroundColor);
	glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end_frame()
{
	// Swap buffers
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	frame_buffer->unbind_object();
	const auto background_color = GlobalSettings::get_global_setting_value<glm::vec4>(GlobalSettingOption::BackgroundColor);
	glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
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

void Renderer::on_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	// Note: position is set to 0 for both x and y coordinates
	frame_buffer->resize(newWidth, newHeight);
	glViewport(0, 0, newWidth, newHeight);
}