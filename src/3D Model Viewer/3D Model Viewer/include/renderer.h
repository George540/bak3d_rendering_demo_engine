#ifndef RENDERER_H
#define RENDERER_H
#include "model.h"

struct GLFWwindow;

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

	static Model* current_model;
	static bool is_grid_rendering;
	static bool is_full_render_selected;
	static bool is_diffuse_render_selected;
	static bool is_specular_selected;
	static bool is_normal_map_selected;

	static Light* environment_point_light;
	static float light_horizontal_rotation;
	static float light_vertical_rotation;
	static float light_origin_distance;
	static glm::vec3 light_diffuse_color;
	static float light_intensity;
};

#endif
