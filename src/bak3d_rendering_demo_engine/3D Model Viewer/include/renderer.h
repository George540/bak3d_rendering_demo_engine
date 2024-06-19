// =====================================================================================
// Renderer class was made by George Mavroeidis using reference for the update function
// only from the COMP 371 lab 10
// =====================================================================================

#ifndef RENDERER_H
#define RENDERER_H

#include <unordered_map>

#include "model.h"
#include <list>

struct GLFWwindow;

struct CurrentModelInfo
{
	Model* current_model;
	std::string model_file_name;
	std::string model_file_path;
	int model_combo_index;

	CurrentModelInfo() : current_model(nullptr), model_file_name("None"), model_file_path("none"), model_combo_index(0) {}

	void set_info(std::string name, std::string path, int index)
	{
		model_file_name = name;
		model_file_path = path;
		model_combo_index = index;
	};
};

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

	static void initialize_imgui();
	static void render_metrics_window();
	static void render_object_window();
	static void render_environment_window();
public:
	static void initialize();
	static void shutdown();

	static void begin_frame();
	static void end_frame();

	static void render_demo_window();

	static bool is_full_render_selected;
	static bool is_diffuse_render_selected;
	static bool is_specular_selected;
	static bool is_normal_map_selected;
	static bool is_gamma_enabled;
	static float shininess;
	static const char* object_combo_items[];
	static int object_current;
	static std::list<std::pair<std::string, std::string>> model_combo_items_list;
	static std::vector<char*> model_combo_items;
	static int model_current;
	static CurrentModelInfo current_model_info;
	static const char* map_combo_items[];
	static int render_current;

	static bool is_grid_rendering;
	static glm::vec3 background_color;
	static Light* environment_point_light;
	static float light_horizontal_rotation;
	static float light_vertical_rotation;
	static float light_origin_distance;
	static glm::vec3 light_diffuse_color;
	static float light_intensity;
};

#endif