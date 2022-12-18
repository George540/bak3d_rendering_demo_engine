#ifndef RENDERER_H
#define RENDERER_H

struct GLFWwindow;

#include <vector>

#include "shader.h"

class Renderer
{
	static GLFWwindow* r_window;

	static void initialize_imgui();
public:
	static void initialize();
	static void shutdown();

	static void begin_frame();
	static void end_frame();

	static void render_demo_window();
};

#endif
