# The Bak3D Rendering Demo Engine
Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

<img width="2498" height="1080" alt="image" src="https://github.com/user-attachments/assets/ec0a7938-b165-4c1b-b515-d384997772b9" />

<p align="center">
  <a href="https://discordapp.com/users/242055319037935616">Add me on Discord</a> •
  <a href="github.com/users/George540/projects/9">Project Board</a> •
  <a href="https://github.com/George540/Bak3DEngine/issues">Issues</a>
</p>

# What is Bak3D

Bak3D (baked) is a real-time 3D rendering engine that can be used as a playground, focusing on simplicity, continous learning, and understanding various rendering concepts. The engine contains simplified features that can be tuned in real time using the engine's editor. It is intended for early learners to use as a reference for their own projects, school assignments or just experimenting.

# Motivation

Bak3D engine started as a fun project back at a time where I just graduated from University in 2022. I had just started my first full-time opportunity as a 3D Graphics Programmer and I wanted to make a project that will supplement my learning experience from that job. Based on an old sample project I made for a Computer Graphics course, Bak3D became an super-extension of it. It became a playground for different rendering and engine concepts I always wanted to learn more about and introduce them to other beginners willing to learn or get help for their school assignments. Although it is a 4 year old engine based on junior knowledge, I occasionally still try to update it when I am looking to rapidly implement something I am interested in playing around with. The purpose of keeping the project in OpenGL because this is usually the first graphics API learners and students are exposed to, thus why it is based on an old school assignment and guide.

![1719347671300](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/3030030b-f6c3-4a0e-ac66-0de793579d5e)
*Bak3D v1.2 introduces more features and tunings, like Particle Systems*


---


## Engine Features

| Feature | Description |
|--------|---------|
| **Editor** | ImGui-based editor with environment settings, object settings for models and particle systems, metrics view and soon to make it hierarchy-based and flexible panels. |
| **Environment** | Simple scene with a single point light, grid and axis overlays for auditing loaded models. |
| **Lighting** | Simple spotlight with Blinn-Phong lighting method implemented. Can tune properties in Environment Settings. |
| **Asset import** | Assimp for loading 40+ model formats, stbi_image for loading image-based formats. |
| **Input** | Keyboard and Mouse: Hold **Left Mouse Button** doiwn and move around to orbit the scene's origin and **Scroll Wheel** to zoom in and out. |
| **Models** | Assimp-loaded custom models that can be textured and use simple Blinn-Phong based shaders for sahding (soon to be PBR). Backpack.mtl is the main sample. |
| **Texture Slicing** | Breakdown and audit existing texture channels for the loaded model. Currently supporting albedo, normal, specular and height map textures. |
| **Particles** | GPU-driven particle system that can render thousands of particles with an extensive property modifications (size, velocity, lifetime, amount, textures, bounding box flow, etc) (restructuring at the moment). |
| **Metrics** | Simple profiling window for displaying basic GPU metrics, such as FPS, frame time in both numerical and histogram formats. |


## Third-Party libraries

Found inside the third_party directory, these are the current libraries being supported and setup locally using CMake.
| Library | Details |
|--------|---------|
| **Assimp** | Universal library for loading 40+ model formats in the engine. |
| **GLAD** | Simple OpenGL header for linking core OpenGL GPU functions to the source code. |
| **GLFW** | Multi-platform support for creating windows, contexts, surfaces, receiving input and events. |
| **GLM** | OpenGL-based header-only math library. |
| **ImGui** | Bloat-free immediate-mode graphical user interface for the engine's editor. |
| **KHRPlatform** | Khronos group header file that defines platform-specific types and macros for OpenGL. |
| **stb_image** | Lightweight header file for loading and creating textures in OpenGL. |


## How to build
Simply clone the repo locally and run the setup.py script located in the root of the repository. Choose your .sln format and configuration and then open the solution once everything is done.

## How to launch a release build
Download and extract the latest .zip folder on the *Releases* section onto your personal computer. Enter the folder and launch the **Bak3D Engine** .exe application. The program will run in windowed version that covers the entire display. To close the program, just open the console window and close it.


## Next Features

### v1.3 - The Abstraction Update
| Feature | Description |
|--------|---------|
| **Class Abstraction** | Create an organized but simple structure for different concepts that are reused often (objects, buffers, materials, shaders, textures, lights, etc). |
| **FrameBuffers** | Surprisingly I never had framebuffers set up, so if I want better control of the final output image, I need that. |
| **ImGui Editor Overhaul** | A more professional and user-fiendly look is needed as the engine grows. Flexible panels and different windows will be added. |
| **Logging System** | Create a universal logging system to output verbose logs, warnings and errors. Will output in ImGui panel. |
| **Asset Viewer** | New panel to view all assets found in content folders, such as models, textures and shaders. |
| **Refined Object Settings** | They are quite hard to understand right now for both models and particles. Currently setting it up just like an ECS-based view. |
| **Post Processing** | Add some basic post processing settings to be applied to the viewport framebuffer Also considering rendering passes. |
| **Profiling Metrics** | Add a dedicated panel for profiling different features and other important metrics. |
| **PBR Lighting** | A fundamental rendering concept I've been looking forward to implement. |
| **Bug fixing** | Fix minor bugs in some features I have left behind. |
| **Code restructuring** | Some more important code overhaul that will be easier to build on top of in the future. |


### v1.4 - The Rendering Universe Expands
| Feature | Description |
|--------|---------|
| **Compute Pipeline** | Add compute pipeline and shaders in the Renderer for opening the possibility for more rendering features. Also implement for GPU Particles. |
| **Light Types** | More light types to add such as spotlights, area lights and directional lights. Support multiple lights in scene and shaders as well. |
| **Polish GPU Particles** | Particle System is barely hanging by. A new and more capable approach is needed. |
| **Multiple Objects in scene** | Support multiple objects in scene, even from the same type. |
| **Terrain** | Simple terrain concept for loading a height map into a plane mesh, used for lighting purposes. |
| **Refined Object Settings** | They are quite hard to understand right now for both models and particles. Currently setting it up just like an ECS-based view. |
| **Post Processing** | Add some basic post processing settings to be applied to the viewport framebuffer. |
| **Scene Graph** | Add a scene graph for proper handling of multiple objects using spacial culling. |
| **GPU Culling** | As the scene grows, we need to handle it at scale with culling unecessary or unseen objects. |
| **PBR Lighting** | A fundamental rendering concept I've been looking forward to implement. |
| **Shadows** | With a complex environment scene, implementing shadows (perhaps CSM) will make it even more lively. |
| **Bonus: Rigidbody demo setup** | If time and motivation, add a simple rigidbody structure for fun physics. |
| **Bonus: FPV movement** | make camera move around the scene without just being restricted in orbit. |

I am open for requests as well as open to any support to help implement these features, although I am willing to try to implement most of these myself whenever I can. Just let me know!


---


## Epilogue

In general, this project is aimed to be very simple with no commercial goal in mind. It is mostly a learning project and a demonstration of my knowledge. If anyone is using it for an academic assignment, project or even personal use, do not hesitate to credit this repository (see license).

I am currently working as a graphics developer at flight simulators and this was a good project to get me on track with my onboarding at work. I hope you enjoy using my little beginner engine and I hope to learn many more things ahead.

## Acknowledgments

1. Special thanks to [Joe de Vries](http://joeydevries.com/#home) for his ebook ["Lean OpenGL"](https://learnopengl.com/About) for teaching me graphics when I was just a student. I initially made this project as a learning tool from his guide for one of my assignments. To attribute his work properly, here is [Joe's twitter](https://twitter.com/JoeyDeVriez.) as well. The code provided on this ebook is licensed under the [CC BY 4.0](https://creativecommons.org/licenses/by-nc/4.0/) license. Proper attribution was given as well as any direct reference of his work in my code. This work is non-commercial.

2. I also appreciate the experts at [Graphics Programming](https://discord.gg/DStdQSaN) and [Together C & C++](https://discord.gg/tccpp) discord servers for answering my basic questions when I needed help understanding something in OpenGL and C++.

3. Finally, thanks to vblanco20-1 for his thorough guide to transitioning to Vulkan a few years later after learning OpenGL. Some of the Vulkan concepts in his guide have been (or will be) translated to this learning project. Attributing to his github repository guide [vkguide.dev](https://vkguide.dev)

