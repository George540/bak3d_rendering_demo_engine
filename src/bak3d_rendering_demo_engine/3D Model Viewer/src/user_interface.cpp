#include "user_interface.h"

#include <iostream>
#include <ranges>

#include "event_manager.h"
#include "renderer.h"
#include "file_loader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "resource_manager.h"
#include "stb_image.h"

using namespace std;

// Environment
bool UserInterface::is_grid_rendering = true;
glm::vec3 UserInterface::background_color = glm::vec3(0.3f);

// Model
bool UserInterface::is_full_render_selected = true;
bool UserInterface::is_diffuse_render_selected = false;
bool UserInterface::is_specular_selected = false;
bool UserInterface::is_normal_map_selected = false;
bool UserInterface::is_gamma_enabled = false;
float UserInterface::shininess = 64.0f;
const char* UserInterface::object_combo_items[] = { "None", "Model", "Particle System", "GPU Particles" };
int UserInterface::object_current = 0;
vector<const char*> UserInterface::model_combo_items; // backpack, mushroom, etc
Model* UserInterface::model_current_object = nullptr;
int UserInterface::model_current_index = 0;
const char* UserInterface::map_combo_items[] = { "Full Render", "Albedo", "Specular", "Normal" };
int UserInterface::render_current = 0;

// Particle system
ParticleSystem* UserInterface::current_particle_system = nullptr;
particle_info UserInterface::particle_payload_info = particle_info();
list<pair<string, string>> UserInterface::particle_image_combo_items_list;
vector<char*> UserInterface::particle_image_combo_items;
int UserInterface::particle_image_current = 1;

// Light
Light* UserInterface::environment_point_light = nullptr;
float UserInterface::light_horizontal_rotation = 0.0f;
float UserInterface::light_vertical_rotation = 0.0f;
float UserInterface::light_origin_distance = 0.0f;
glm::vec3 UserInterface::light_diffuse_color = glm::vec3(1.0f);
float UserInterface::light_intensity = 1.0f;

void UserInterface::initialize()
{
    initialize_imgui();
	
	model_combo_items.emplace_back("None");
	list<pair<string, string>> models_file_path_names;
	for (auto model_pair : ResourceManager::Models)
	{
		models_file_path_names.emplace_back(model_pair.first, model_pair.second->get_path());
	}
	auto model_file_names = FileLoader::get_vector_items_to_array(models_file_path_names, false);

	for (auto file_name : model_file_names)
	{
		model_combo_items.push_back(file_name);
	}

	for (auto [texture_file_name, texture_path] : ResourceManager::Textures)
	{
		if (texture_path->get_texture_use_type() == TextureUseType::Particle)
		{
			//particle_image_combo_items.emplace_back(texture_file_name);
		}
	}
}

void UserInterface::initialize_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Renderer::get_window(), true);
    const auto glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    cout << "Initializing IMGUI window..." << endl;
}

void UserInterface::begin_frame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::render_demo_window()
{
    render_metrics_window();
    render_environment_window();
    render_object_window();
}

void UserInterface::render_metrics_window()
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

    // Fill an array with contiguous float values to plot
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

void UserInterface::render_environment_window()
{
    // ENVIRONMENT SETTINGS
    ImGui::Begin("Environment Settings");

    ImGui::Checkbox("Render Grid", &is_grid_rendering);
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

void UserInterface::render_object_window()
{
	// OBJECT SETTINGS WINDOW;
	ImGui::Begin("Object Settings");

	// Object Selection Dropdown
	// Simplified one-liner Combo() using an accessor function
	ImGui::Combo("Object Selection", &object_current, object_combo_items, IM_ARRAYSIZE(object_combo_items));

	if (object_current == 1)
	{
		auto updated_model = ResourceManager::get_model(model_combo_items[model_current_index]);
		if (updated_model && model_current_object && updated_model->get_asset_name() != model_current_object->get_asset_name())
		{
			model_current_object->set_visible(false);
			model_current_object = updated_model;
			model_current_object->set_visible(true);
		}
		ImGui::Combo("Model Selection", &model_current_index, model_combo_items.data(), static_cast<int>(model_combo_items.size()));

		if (model_current_index > 0)
		{
			auto current_model = ResourceManager::get_model(model_combo_items[model_current_index]);
			current_model->set_visible(true);
			
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
					current_model->set_current_toggle_shader(0);
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
					current_model->set_current_toggle_shader(1);
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
					current_model->set_current_toggle_shader(1);
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
					current_model->set_current_toggle_shader(1);
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
		model_current_index = 0;
	}

	ImGui::End();
}

void UserInterface::end_frame()
{
	// Render ImGui window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Free up particle image combo items stored in memory
	for (size_t i = 0; i < particle_image_combo_items_list.size(); ++i)
	{
		delete[] particle_image_combo_items[i];
	}
}