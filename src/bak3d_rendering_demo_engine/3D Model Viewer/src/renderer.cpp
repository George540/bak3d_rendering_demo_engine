#include <iostream>
#include <filesystem>
#include <cstring>

#include "renderer.h"
#include "event_manager.h"
#include "file_loader.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "stb_image.h"


using namespace std;

// Renderer
GLFWwindow* Renderer::r_window = nullptr;
GLuint Renderer::frame_buffer = NULL;
GLuint Renderer::texture_color_buffer = NULL;
GLuint Renderer::render_buffer = NULL;

// Environment
bool Renderer::is_grid_rendering = true;
glm::vec3 Renderer::background_color = glm::vec3(0.3f);;

// Model
CurrentModelInfo Renderer::current_model_info = CurrentModelInfo();
bool Renderer::is_full_render_selected = true;
bool Renderer::is_diffuse_render_selected = false;
bool Renderer::is_specular_selected = false;
bool Renderer::is_normal_map_selected = false;
bool Renderer::is_gamma_enabled = false;
float Renderer::shininess = 64.0f;
const char* Renderer::object_combo_items[] = { "None", "Model", "Particle System", "GPU Particles" };
int Renderer::object_current = 0;
list<pair<string, string>> Renderer::model_combo_items_list;
vector<char*> Renderer::model_combo_items;
int Renderer::model_current = 0;
const char* Renderer::map_combo_items[] = { "Full Render", "Albedo", "Specular", "Normal" };
int Renderer::render_current = 0;

// Particle system
ParticleSystem* Renderer::current_particle_system = nullptr;
particle_info Renderer::particle_payload_info = particle_info();
list<pair<string, string>> Renderer::particle_image_combo_items_list;
vector<char*> Renderer::particle_image_combo_items;
int Renderer::particle_image_current = 1;

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
	cout << "Capturing window at Renderer..." << endl;

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		cerr << "Failed to initialize GLAD" << endl;
		exit(-1);
	}
	cout << "Initializing GLAD..." << endl;

	glfwSetFramebufferSizeCallback(r_window, EventManager::framebuffer_size_callback);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
	glGetError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	cout << "Enabling depth test..." << endl;

	initialize_imgui();

	// Load list of model assets found in assets folder and turn them into combo items vectors
	model_combo_items_list = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets"), FileType::obj);
	model_combo_items_list.push_front(make_pair("None", "none"));
	model_combo_items = FileLoader::get_vector_items_to_array(model_combo_items_list);

	// Load list of particle texture images found in particles-textures folder and turn them into combo items vectors
	particle_image_combo_items_list = FileLoader::get_files_by_type_with_path(filesystem::absolute("assets/particles-textures"), FileType::png);
	particle_image_combo_items = FileLoader::get_vector_items_to_array(particle_image_combo_items_list);

	cout << "Ending Renderer Initialization..." << endl;
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

	cout << "Initializing IMGUI window..." << endl;
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

	// Background Color
	glClearColor(background_color.r, background_color.g, background_color.b, 0.0f);
}

void Renderer::render_demo_window()
{
	render_metrics_window();
	render_environment_window();
	render_object_window();
}

void Renderer::render_metrics_window()
{
	// METRICS WINDOW
	ImGui::Begin("Metrics");
	float imgui_framerate = ImGui::GetIO().Framerate;
	float frame_time = EventManager::get_frame_time();
	int fps = EventManager::get_frames_per_second();
	ImGui::Text("Application average %.3f ms/frame", 1000.0f / imgui_framerate);
	ImGui::Text("ImGuiIO: %.1f FPS", imgui_framerate);
	ImGui::Text("OpenGL FPS: %d", fps);
	ImGui::Text("EventManager: %.4f ms/frame", 1000.0f * frame_time);

	// Fill an array of contiguous float values to plot
	static float values[90] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;

	if (refresh_time == 0.0)
	{
		refresh_time = ImGui::GetTime();
	}

	while (refresh_time < ImGui::GetTime()) // Create data based on the OpenGL frame rate
	{
		static float phase = 0.0f;
		values[values_offset] = frame_time * 1000.0f;
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / fps;
	}

	if (values[values_offset] > EventManager::get_frame_time_max())
	{
		EventManager::set_frame_time_max(values[values_offset]);
	}
	char overlay[32];
	sprintf_s(overlay, "max: %.4f", EventManager::get_frame_time_max());
	ImGui::PlotHistogram("##", values, IM_ARRAYSIZE(values), values_offset, overlay, -100.0f, 100.0f, ImVec2(0, 80.0f));

	ImGui::End();
}

void Renderer::render_environment_window()
{
	// ENVIRONMENT SETTINGS
	ImGui::Begin("Environment Settings");

	ImGui::Checkbox("Render Grid", &Renderer::is_grid_rendering);
	// Toggle light color
	float bg_col[3] = { background_color.r, background_color.g, background_color.b };
	ImGui::ColorEdit3("Background Color", bg_col);
	background_color.r = bg_col[0];
	background_color.g = bg_col[1];
	background_color.b = bg_col[2];
	environment_point_light->set_diffuse_color(background_color);

	if (is_full_render_selected)
	{
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
		float light_col[3] = { light_diffuse_color.r, light_diffuse_color.g, light_diffuse_color.b };
		ImGui::ColorEdit3("Light Color", light_col);
		light_diffuse_color.r = light_col[0];
		light_diffuse_color.g = light_col[1];
		light_diffuse_color.b = light_col[2];
		environment_point_light->set_diffuse_color(light_diffuse_color);
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.3f), "Light disabled. Object is in preview mode.");
	}

	ImGui::End();
}

void Renderer::render_object_window()
{
	// OBJECT SETTINGS WINDOW;
	ImGui::Begin("Object Settings");

	// Object Selection Dropdown
	// Simplified one-liner Combo() using an accessor function
	ImGui::Combo("Object Selection", &object_current, object_combo_items, IM_ARRAYSIZE(object_combo_items));

	if (object_current == 1)
	{
		ImGui::Combo("Model Selection", &model_current, model_combo_items.data(), static_cast<int>(model_combo_items.size()));
		string current_model_name = model_combo_items[model_current];

		// Find currently selected dropdown item into list and store its dropdown info
		auto model_info = find_if(model_combo_items_list.begin(), model_combo_items_list.end(), [&current_model_name](const auto& pair)
			{
				return pair.first == current_model_name;
			});
		if (model_info != model_combo_items_list.end())
		{
			current_model_info.set_info(model_info->first, model_info->second, model_current);
		}

		if (current_model_info.current_model && model_current != 0)
		{
			auto current_model = current_model_info.current_model;

			ImGui::Text("Vertices: %d", current_model->m_num_vertices);
			//ImGui::Text("Edges: %d", current_model->m_unique_edges.size());
			ImGui::Text("Faces: %d", current_model->m_num_faces);
			ImGui::Text("Triangles: %d", current_model->m_num_triangles);

			// Toggle map breakdowns
			ImGui::Text("Render Breakdown");
			ImGui::Checkbox("Albedo", &EventManager::is_using_diffuse_texture);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Specular", &EventManager::is_using_specular_texture);
			ImGui::Checkbox("Normal", &EventManager::is_using_normals_texture);

			// Toggle Material Settings
			ImGui::Text("Material Properties");
			ImGui::Checkbox("Gamma Correction", &is_gamma_enabled);
			ImGui::SliderFloat("Shininess", &shininess, 0.0f, 256.0f);


			// Toggle Texture Maps
			ImGui::Text("Texture Map View Selection");
			ImGui::Combo("Map Selection", &render_current, map_combo_items, IM_ARRAYSIZE(map_combo_items));
			if (render_current == 0 && !is_full_render_selected)
			{
				is_full_render_selected = true;
				is_diffuse_render_selected = false;
				is_specular_selected = false;
				is_normal_map_selected = false;

				EventManager::is_using_diffuse_texture = true;
				EventManager::is_using_specular_texture = true;
				EventManager::is_using_normals_texture = true;

				if (current_model)
				{
					current_model->set_current_shader(0);
				}

				cout << "Full Render View" << endl;
			}
			if (render_current == 1 && !is_diffuse_render_selected)
			{
				is_full_render_selected = false;
				is_diffuse_render_selected = true;
				is_specular_selected = false;
				is_normal_map_selected = false;

				EventManager::is_using_diffuse_texture = false;
				EventManager::is_using_specular_texture = false;
				EventManager::is_using_normals_texture = false;

				if (current_model)
				{
					current_model->set_current_shader(1);
				}
				cout << "Albedo Preview" << endl;
			}
			if (render_current == 2 && !is_specular_selected)
			{
				is_full_render_selected = false;
				is_diffuse_render_selected = false;
				is_specular_selected = true;
				is_normal_map_selected = false;

				EventManager::is_using_diffuse_texture = false;
				EventManager::is_using_specular_texture = false;
				EventManager::is_using_normals_texture = false;

				if (current_model)
				{
					current_model->set_current_shader(1);
				}
				cout << "Specular Map Preview" << endl;
			}
			if (render_current == 3 && !is_normal_map_selected)
			{
				is_full_render_selected = false;
				is_diffuse_render_selected = false;
				is_specular_selected = false;
				is_normal_map_selected = true;

				EventManager::is_using_diffuse_texture = false;
				EventManager::is_using_specular_texture = false;
				EventManager::is_using_normals_texture = false;

				if (current_model)
				{
					current_model->set_current_shader(1);
				}
				cout << "Normal Map Preview" << endl;
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.3f), "Model settings disabled. Select imported model to view.");
		}
	}
	else if (object_current == 2)
	{
		ImGui::DragInt("Generator Amount", &particle_payload_info.amount, 1, 1, 10000);

		ImGui::DragFloat("Particle Lifetime", &particle_payload_info.lifetime, 0.01f, 0.01f, 100.0f);
		ImGui::Checkbox("Randomize Lifetime?", &particle_payload_info.randomize_lifetime);
		if (particle_payload_info.randomize_lifetime)
		{
			ImGui::SliderFloat("Lifetime Offset", &particle_payload_info.lifetime_random_offset, 0.0f, particle_payload_info.lifetime);
		}
		else
		{
			particle_payload_info.lifetime_random_offset = 0.0f;
		}

		ImGui::Checkbox("Randomize Rotation?", &particle_payload_info.randomize_rotation);
		if (!particle_payload_info.randomize_rotation)
		{
			ImGui::SliderFloat("Rotation", &particle_payload_info.rotation, 0.0f, 360.0f);
		}
		else
		{
			particle_payload_info.rotation = 0.0f;
		}

		ImGui::Checkbox("Randomize Velocity?", &particle_payload_info.randomize_velocity);
		if (particle_payload_info.randomize_velocity)
		{
			float velocity[3] = { particle_payload_info.velocity_random_offset.x, particle_payload_info.velocity_random_offset.y, particle_payload_info.velocity_random_offset.z };
			ImGui::DragFloat3("Velocity Random Offsets", &velocity[0], 0.1f, 0.1f, 100.0f);
			particle_payload_info.velocity_random_offset.x = velocity[0];
			particle_payload_info.velocity_random_offset.y = velocity[1];
			particle_payload_info.velocity_random_offset.z = velocity[2];
		}
		else
		{
			float velocity[3] = { particle_payload_info.velocity.x, particle_payload_info.velocity.y, particle_payload_info.velocity.z };
			ImGui::DragFloat3("Velocity", &velocity[0], 0.1f, -100.0f, 100.0f);
			particle_payload_info.velocity.x = velocity[0];
			particle_payload_info.velocity.y = velocity[1];
			particle_payload_info.velocity.z = velocity[2];
			
		}

		ImGui::Checkbox("Render Bounding Box", &particle_payload_info.render_bounding_box);
		
		if (!particle_payload_info.randomize_color)
		{
			float bg_col[3] = { particle_payload_info.color.r, particle_payload_info.color.g, particle_payload_info.color.b };
			ImGui::ColorEdit3("Color", bg_col);
			particle_payload_info.color.r = bg_col[0];
			particle_payload_info.color.g = bg_col[1];
			particle_payload_info.color.b = bg_col[2];
		}
		ImGui::Checkbox("Randomize Color?", &particle_payload_info.randomize_color);

		ImGui::Checkbox("Fade Particles?", &particle_payload_info.is_color_faded);
		if (particle_payload_info.is_color_faded)
		{
			ImGui::SliderFloat("Initial Transparency", &particle_payload_info.color.a, 0.0f, 1.0f);
		}
		else
		{
			ImGui::SliderFloat("Transparency", &particle_payload_info.color.a, 0.0f, 1.0f);
		}

		ImGui::SliderFloat("Scale", &particle_payload_info.scale, 0.1f, 20.0f);
		ImGui::Checkbox("Randomize Scale?", &particle_payload_info.randomize_scale);
		if (particle_payload_info.randomize_scale)
		{
			ImGui::SliderFloat("Scale Offset", &particle_payload_info.scale_random_offset, 0.1f, particle_payload_info.scale);
		}
		else
		{
			particle_payload_info.scale_random_offset = 0.1f;
		}
		ImGui::SliderFloat("Range", &particle_payload_info.range, 0.0f, 10.0f);

		ImGui::Combo("Image Selection", &particle_payload_info.texture_selection, particle_image_combo_items.data(), static_cast<int>(particle_image_combo_items.size()));
		
		if (current_particle_system)
		{
			current_particle_system->particles_payload_info = particle_payload_info;
		}
	}
	else if (object_current == 3)
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.3f), "GPU Particles has been selected. Do stuff here too!");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.3f), "Object settings disabled. Selected an object type to view.");
		if (!current_model_info.current_model)
		{
			current_model_info.current_model = nullptr;
			model_current = 0;
		}
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

	// Free up model combo items stored in memory
	for (size_t i = 0; i < model_combo_items_list.size(); ++i)
	{
		delete[] model_combo_items[i];
	}

	// Free up particle image combo items stored in memory
	for (size_t i = 0; i < particle_image_combo_items_list.size(); ++i)
	{
		delete[] particle_image_combo_items[i];
	}

	// Managed by EventManager
	r_window = nullptr;
}