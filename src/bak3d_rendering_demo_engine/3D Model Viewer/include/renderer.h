// =====================================================================================
// Renderer class was made by George Mavroeidis using reference for the update function
// only from the COMP 371 lab 10
// =====================================================================================

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

struct GLFWwindow;

/**
 * \brief Class is static, so it's globally used and there is only one copy of it
 */
class Renderer
{
private:
	static GLFWwindow* r_window;
	static GLuint frame_buffer;
	static GLuint texture_color_buffer;
	static GLuint render_buffer;
public:
	static void initialize();
	static void shutdown();

	static void begin_frame();
	static void end_frame();

	static void render_demo_window();
	static GLFWwindow* get_window() { return r_window; }
};

#endif