#include <iostream>

#include "renderer.h"
#include "event_manager.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "scene.h"
#include "stb_image.h"


using namespace std;

// Renderer
GLFWwindow* Renderer::r_window = nullptr;
GLuint Renderer::frame_buffer = NULL;
GLuint Renderer::texture_color_buffer = NULL;
GLuint Renderer::render_buffer = NULL;

// Model
Model* Renderer::current_model = nullptr;
bool Renderer::is_grid_rendering = true;
bool Renderer::is_full_render_selected = true;
bool Renderer::is_diffuse_render_selected = false;
bool Renderer::is_specular_selected = false;
bool Renderer::is_normal_map_selected = false;
bool Renderer::is_gamma_enabled = false;
float Renderer::shininess = 64.0f;

// Light
Light* Renderer::environment_point_light = nullptr;
float Renderer::light_horizontal_rotation = 0.0f;
float Renderer::light_vertical_rotation = 0.0f;
float Renderer::light_origin_distance = 0.0f;
glm::vec3 Renderer::light_diffuse_color = glm::vec3(1.0f);
float Renderer::light_intensity = 1.0f;


void Renderer::initialize()
{
	r_window = EventManager::get_window();
	glfwMakeContextCurrent(r_window);
	std::cout << "Capturing window at Renderer..." << endl;

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "Initializing GLAD..." << std::endl;

	glfwSetFramebufferSizeCallback(r_window, EventManager::framebuffer_size_callback);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Gray background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	std::cout << "Enabling depth test..." << endl;

	initialize_imgui();

	std::cout << "Ending Renderer Initialization..." << endl;
}

void Renderer::initialize_imgui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(r_window, true);
	const auto glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	std::cout << "Initializing IMGUI window..." << endl;
}

void Renderer::begin_frame()
{
	// Clear the screen
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Renderer::render_demo_window()
{
	// METRICS WINDOW
	ImGui::Begin("Metrics");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Application average %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("ImGuiIO: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("EventManager: %.4f ms/frame", 1000.0f * EventManager::get_frame_time());
	ImGui::End();

	// SETTINGS WINDOW
	ImGui::Begin("Settings");

	// Toggle Environment graphics
	ImGui::Text("Environment Toggles");
	ImGui::Checkbox("Render Grid", &Renderer::is_grid_rendering);

	// Toggle Light properties: position, color, intensity
	ImGui::Text("Light Repositioning");
	ImGui::SliderFloat("Horizontal", &light_horizontal_rotation, 0.0f, 360.0f);
	environment_point_light->set_horizontal_angle(light_horizontal_rotation);
	ImGui::SliderFloat("Vertical", &light_vertical_rotation, 0.0f, 360.0f);
	environment_point_light->set_vertical_angle(light_vertical_rotation);
	ImGui::SliderFloat("Distance", &light_origin_distance, 2.0f, 10.0f);
	environment_point_light->set_distance_offset(light_origin_distance);
	ImGui::SliderFloat("Intensity", &light_intensity, 0.0f, 5.0f);
	environment_point_light->set_light_intensity(light_intensity);

	// Toggle light color
	float light_col[3] = {light_diffuse_color.r, light_diffuse_color.g, light_diffuse_color.b};
	ImGui::ColorEdit3("Light Color", light_col);
	light_diffuse_color.r = light_col[0];
	light_diffuse_color.g = light_col[1];
	light_diffuse_color.b = light_col[2];
	environment_point_light->set_diffuse_color(light_diffuse_color);

	// Toggle map breakdowns
	ImGui::Text("Render Breakdown"); // Display some text (you can use a format strings too)
	ImGui::Checkbox("Albedo", &EventManager::is_using_diffuse_texture);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Specular", &EventManager::is_using_specular_texture);
	ImGui::Checkbox("Normal", &EventManager::is_using_normal_maps);

	// Toggle Material Settings
	ImGui::Text("Material Properties");
	ImGui::Checkbox("Gamma Correction", &is_gamma_enabled);
	ImGui::SliderFloat("Shininess", &shininess, 0.0f, 256.0f);


	// Toggle Texture Maps
	ImGui::Text("Texture Map Toggles");
	if (ImGui::Button("Full Render"))
	{
		is_full_render_selected = true;
		is_diffuse_render_selected = false;
		is_specular_selected = false;
		is_normal_map_selected = false;

		EventManager::is_using_diffuse_texture = true;
		EventManager::is_using_specular_texture = true;
		EventManager::is_using_normal_maps = true;

		current_model->set_current_shader(0);
		std::cout << "Full Render Mode" << std::endl;
	}
	if (ImGui::Button("Albedo"))
	{
		is_full_render_selected = false;
		is_diffuse_render_selected = true;
		is_specular_selected = false;
		is_normal_map_selected = false;

		EventManager::is_using_diffuse_texture = false;
		EventManager::is_using_specular_texture = false;
		EventManager::is_using_normal_maps = false;

		current_model->set_current_shader(1);
		std::cout << "Albedo Dissecting" << std::endl;
	}
	if (ImGui::Button("Specular"))
	{
		is_full_render_selected = false;
		is_diffuse_render_selected = false;
		is_specular_selected = true;
		is_normal_map_selected = false;

		EventManager::is_using_diffuse_texture = false;
		EventManager::is_using_specular_texture = false;
		EventManager::is_using_normal_maps = false;

		current_model->set_current_shader(1);
		std::cout << "Specular Map Dissecting" << std::endl;
	}
	if (ImGui::Button("Normal"))
	{
		is_full_render_selected = false;
		is_diffuse_render_selected = false;
		is_specular_selected = false;
		is_normal_map_selected = true;

		EventManager::is_using_diffuse_texture = false;
		EventManager::is_using_specular_texture = false;
		EventManager::is_using_normal_maps = false;

		current_model->set_current_shader(1);
		std::cout << "Rendering Dissecting" << std::endl;
	}
	ImGui::End();
}

void Renderer::end_frame()
{
	// Render ImGui window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap buffers
	glDisable(GL_DEPTH_TEST);
	glfwSwapBuffers(r_window);
	glfwPollEvents();
}

void Renderer::shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Managed by EventManager
	r_window = nullptr;
}