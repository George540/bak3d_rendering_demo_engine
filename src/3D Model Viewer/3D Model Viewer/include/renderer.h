#ifndef RENDERER_H
#define RENDERER_H

struct GLFWwindow;

#include <vector>

#include "shader.h"

class Renderer
{
	static GLFWwindow* r_window;
	static std::vector<Shader> shader_program_id;
	static unsigned int current_shader;
public:
	static void initialize();
	static void shutdown();

	static void begin_frame();
	static void end_frame();

	static void check_for_errors();
	static bool print_error();
};

#endif
