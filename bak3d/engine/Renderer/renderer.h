// =====================================================================================
// Renderer class was made by George Mavroeidis using reference for the update function
// only from the COMP 371 lab 10
// =====================================================================================

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include "buffer.h"

struct GLFWwindow;

/**
 * \brief Class is static, so it's globally used and there is only one copy of it
 */
class Renderer
{
private:
	static GLFWwindow* r_window;
	static FrameBuffer* frame_buffer;
public:
	static void initialize();
	static void shutdown();

	static void begin_frame();
	static void end_frame();
	static void post_end_frame();

	static GLFWwindow* get_window() { return r_window; }
	static FrameBuffer* get_frame_buffer() { return frame_buffer; }

	static void on_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight);
};

#endif