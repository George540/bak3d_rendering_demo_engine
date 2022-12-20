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

GLFWwindow* Renderer::r_window = nullptr;
Model* Renderer::current_model = nullptr;
bool Renderer::is_grid_rendering = true;
bool Renderer::is_full_render_selected = true;
bool Renderer::is_diffuse_render_selected = false;
bool Renderer::is_specular_selected = false;
bool Renderer::is_normal_map_selected = false;


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

	//check_for_errors();
	std::cout << "Ending Renderer Initialization..." << endl;
}

void Renderer::initialize_imgui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Renderer::render_demo_window()
{
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Metrics");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Application average %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("ImGuiIO: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("EventManager: %.4f ms/frame", 1000.0f * EventManager::get_frame_time());
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::Text("Environment Toggles");
	ImGui::Checkbox("Render Grid", &Renderer::is_grid_rendering);
	ImGui::Text("Render Breakdown"); // Display some text (you can use a format strings too)
	ImGui::Checkbox("Albedo", &EventManager::is_using_diffuse_texture);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Specular", &EventManager::is_using_specular_texture);
	ImGui::Checkbox("Normal", &EventManager::is_using_normal_maps);
	ImGui::End();

	ImGui::Begin("Map Breakdowns");
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

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color
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