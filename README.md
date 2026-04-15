# The Bak3D Rendering Demo Engine
Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

<img width="1200" height="657" alt="image" src="https://github.com/user-attachments/assets/1d002028-8b45-4fee-86a2-e6abab5f9133" />


# What is Bak3D

Bak3D (baked) is a real-time beginner-friendly 3D rendering engine that can be used as a playground, focusing on simplicity, continous learning, and understanding various rendering concepts. The engine contains simplified features that can be tuned in real time using the engine's editor. It is intended for early learners to use as a reference for their own projects, school assignments or just experimenting. It is also good for seasoned professionals who love simple things from simple times. So if you are a student or just starting to understand computer graphics and engine, this one is especially for you!

# Motivation

Bak3D engine started as a fun project back at a time where I just graduated from University in 2022. I had just started my first full-time opportunity as a Junior 3D Graphics Programmer and I wanted to make a project that will supplement my learning experience from that job. Based on an old sample project I made for a Computer Graphics course, Bak3D became an super-extension of it. It formed a playground for different rendering and engine concepts I always wanted to learn more about and introduce them to other beginners willing to learn or get help for their school assignments. Although it is a 4 year old engine based on junior knowledge, I occasionally still try to update it when I am looking to rapidly implement something I am interested in playing around with.


<img width="2498" height="1080" alt="image" src="https://github.com/user-attachments/assets/ec0a7938-b165-4c1b-b515-d384997772b9" />

*Bak3D v1.2 introduces more features and tunings, like Particle Systems*

# This engine is for you if you are...

- :ballot_box_with_check: ...a rendering enthusiast and are interested how video games look like behind the hood.
- :ballot_box_with_check: ...an aspiring beginner who just got introduced to the concept of computer graphics.
- :ballot_box_with_check: ...a student currently taking a course in Computer Graphics in school, university or on even online.
- :ballot_box_with_check: ...completely lost about this topic, whether your instructor is not helping at all or just finding it hard to put the pieces together.
- :ballot_box_with_check: ...an experienced individual in making video games and/or engines and are willing to help newcomers explore the craft.
- :ballot_box_with_check: ...looking to get a starting point or reference for making your first engine.

This is (or planning to be) the perfect starting point for computer graphics and graphics programming. The purpose of this engine is to assist students and beginners on understanding the behind-the-scenes for different rendering phenomena. It is also a great starting point for creating their own project (under licensing conditions). It is made in OpenGL since it is usually the go-to library for most of introduction courses and to simplify most of the rendering processes. This helps with putting a focus on the actual learning material rather than the API itself.

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

<img width="2498" height="1080" alt="image" src="https://github.com/user-attachments/assets/3639aed6-ff0f-463a-999b-daf375aa9b83" />

*Custom Mushroom model imported into the engine with appropriate textures.*



## Third-Party libraries

Found inside the third_party directory, these are the current libraries being supported and setup locally using CMake.
| Library | Details |
|--------|---------|
| **[Assimp](https://github.com/assimp/assimp)** | Universal library for loading 40+ model formats in the engine. |
| **[Glad](https://glad.dav1d.de)** | Simple OpenGL generated header for linking core OpenGL GPU functions to the source code. |
| **[GLFW](https://www.glfw.org)** | Multi-platform support for creating windows, contexts, surfaces, receiving input and events. |
| **[GLM](https://github.com/g-truc/glm)** | OpenGL-based header-only math library. |
| **[ImGui](https://github.com/ocornut/imgui)** | Bloat-free immediate-mode graphical user interface for the engine's editor. |
| **[KHRPlatform](https://registry.khronos.org/EGL/api/KHR/khrplatform.h)** | Khronos group header file that defines platform-specific types and macros for OpenGL. |
| **[stb_image](https://github.com/nothings/stb)** | Lightweight header file for loading and creating textures in OpenGL, by stb. |


## How to build
Simply clone the repo locally and run the setup.py script located in the root of the repository. Choose your .sln format and configuration and then open the solution once everything is done.

## How to launch a release build
Download and extract the latest .zip folder on the *Releases* section onto your personal computer. Enter the folder and launch the **Bak3D Engine** .exe application. The program will run in windowed version that covers the entire display. To close the program, just open the console window and close it.


## Roadmap

### [v1.3 - The Abstraction Update](https://github.com/George540/Bak3DEngine/milestone/2)
| Status | Feature | Description |
| :---: | :--- | :--- |
|:ballot_box_with_check:| **Class Abstraction** | Create an organized but simple structure for different concepts that are reused often (objects, buffers, materials, shaders, textures, lights, etc). |
|:ballot_box_with_check:| **FrameBuffers** | Surprisingly I never had framebuffers set up, so if I want better control of the final output image, I need that. |
|:gear:| **ImGui Editor Overhaul** | A more professional and user-fiendly look is needed as the engine grows. Flexible panels and different windows will be added. |
|:ballot_box_with_check:| **Logging System** | Create a universal logging system to output verbose logs, warnings and errors. Will output in ImGui panel. |
|:ballot_box_with_check:| **Asset Viewer** | New panel to view all assets found in content folders, such as models, textures and shaders. |
|:ballot_box_with_check:| **Refined Object Settings** | They are quite hard to understand right now for both models and particles. Currently setting it up just like an ECS-based view. |
|:gear:| **Post Processing** | Add some basic post processing settings to be applied to the viewport framebuffer Also considering rendering passes. |
|:ballot_box_with_check:| **Profiling Metrics** | Add a dedicated system and panel for profiling different features and other important metrics. |
|☐| **Light Types** | More light types to add such as spotlights, area lights and directional lights. For now, just make the single light in the scene to change type. |
|:gear:| **Bug fixing** | Fix minor bugs in some features I have left behind. |
|:gear:| **Code restructuring** | Some more important code overhaul that will be easier to build on top of in the future. |
|☐| **Bonus: QoL Features** | Add shader recompiling at runtime to test shader changes faster and integrate RenderDoc for rapid GPU captures. |


### v1.4 - The Rendering Universe Expands
| Status | Feature | Description |
| :---: | :--- | :--- |
|☐| **Shadows** | With a complex environment scene, implementing shadows (perhaps CSM) will make it even more lively. |
|☐| **PBR Lighting** | A fundamental rendering concept I've been looking forward to implement. PBR Materials as well as HDR and IBL. |
|☐| **Deferred Rendering** | Apply the possibility to integrate rendering features at scale and keep it industry standard. Opens up the possibility for multiple lights in scene. |
|☐| **Compute Pipeline** | Add compute pipeline and shaders in the Renderer for opening the possibility for more rendering features. Also implement for GPU Particles. |
|☐| **Polish GPU Particles** | Particle System is barely keeping up. A new and more capable approach is needed. |
|☐| **Multiple Objects in scene** | Support multiple objects in scene, even from the same type. |
|☐| **Terrain** | Simple terrain concept for loading a height map into a plane mesh, used for lighting purposes. |
|☐| **Scene Graph** | Add a scene graph for proper handling of multiple objects using spacial culling. |
|☐| **GPU Culling** | As the scene grows, we need to handle it at scale with culling unecessary or unseen objects. |
|☐| **Bonus: Rigidbody demo setup** | If time and motivation, add a simple rigidbody structure for fun physics. |
|☐| **Bonus: FPV movement** | make camera move around the scene without just being restricted in orbit. |

I am open for requests as well as open to any support to help implement these features, although I am willing to try to implement most of these myself whenever I can. Just let me know!

See [project board](github.com/users/George540/projects/9) and [issue tracking](https://github.com/George540/Bak3DEngine/issues) for more details on what's currently being developed.

---


## Epilogue

In general, this project is aimed to be very simple with no commercial goal in mind. It is mostly a learning project and a demonstration of my knowledge. If anyone is using it for an academic assignment, project or even personal use, do not hesitate to credit this repository (see license).

I hope you enjoy using my little beginner engine and I hope you learn many more things with me as well.

## Acknowledgments

1. Special thanks to [Joe de Vries](http://joeydevries.com/#home) for his ebook ["Lean OpenGL"](https://learnopengl.com/About) for teaching me graphics when I was just a student. I initially made this project as a learning tool from his guide for one of my assignments. To attribute his work properly, here is [Joe's twitter](https://twitter.com/JoeyDeVriez.) as well. The code provided on this ebook is licensed under the [CC BY 4.0](https://creativecommons.org/licenses/by-nc/4.0/) license. Proper attribution was given as well as any direct reference of his work in my code. This work is non-commercial.

2. I also appreciate the experts at [Graphics Programming](https://discord.gg/DStdQSaN) and [Together C & C++](https://discord.gg/tccpp) discord servers for answering my basic questions when I needed help understanding something in OpenGL and C++.

3. Finally, thanks to [vblanco20-1](https://github.com/vblanco20-1) for his thorough guide to transitioning to Vulkan a few years later after learning OpenGL. Some of the Vulkan concepts in his guide have been (or will be) translated to this learning project. Attributing to his github repository guide [vkguide.dev](https://vkguide.dev)

## License

**MIT License**, free to use with attribution.


