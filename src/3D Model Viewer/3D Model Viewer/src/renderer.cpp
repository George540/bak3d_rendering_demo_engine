#include <iostream>

#include "renderer.h"
#include "event_manager.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "stb_image.h"


using namespace std;

GLFWwindow* Renderer::r_window = nullptr;

bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


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

	ImGui::Begin("Metrics!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color

	if (ImGui::Button("Albedo Toggle"))                          // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		EventManager::set_using_diffuse_texture(!EventManager::get_using_diffuse_texture());
	}
	ImGui::SameLine();
	ImGui::Text("Is On: %d\n", EventManager::get_using_diffuse_texture());

	if (ImGui::Button("Specular Toggle"))                          // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		EventManager::set_using_specular_texture(!EventManager::get_using_specular_texture());
	}
	ImGui::SameLine();
	ImGui::Text("Is On: %d\n", EventManager::get_using_specular_texture());

	if (ImGui::Button("Normal Toggle"))                          // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		EventManager::set_using_normal_maps(!EventManager::get_using_normal_maps());
	}
	ImGui::SameLine();
	ImGui::Text("Is On: %d\n", EventManager::get_using_normal_maps());

	ImGui::Text("Application average %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("ImGuiIO: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("EventManager: %.4f ms/frame", 1000.0f * EventManager::get_frame_time());
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