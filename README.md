# The Bak3D Rendering Demo Engine
Made by George Mavroeidis

## Motivation
Computer graphics has always been one of my most beloved subjects in computer science and in games. I like solving complex visual problems and find solutions to make them even better than before. What makes me like computer graphics is making things become more optimized through mathematics, logic and technical knowledge. This project is a simple graphics engine that I use as a playground to learn and implement new topics and features I am learning throughout my career. It started as a simple project in 2022 and now it has become something bigger and multipurpose. Anything computer graphics goes here!

## What is Bak3D
Bak3D (baked) is a real-time 3D rendering engine that can be used as a playground for different 3D concepts. The engine contains simplified features that can be tuned in real time based on the user's preferences.

![1719347671300](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/3030030b-f6c3-4a0e-ac66-0de793579d5e)
*Bak3D v1.2 introduces more features and tunings, like Particle Systems*

## How it's made
The project is made in C++ and [OpenGL](https://www.opengl.org). Additionally, the [Assimp library](https://github.com/assimp/assimp) is used to assist with model loading and [Dear ImGui](https://github.com/ocornut/imgui) for the user interface. As you might have noticed, all these tools are open-source and free to use. More details on the Aknowledgements section.

## How to launch
Download and extract the latest .zip folder on the *Releases* section onto your personal computer. Enter the folder and launch the **Bak3D Engine** .exe application. The program will run in windowed version that covers the entire display. To close the program, just open the console window and close it.

## Features
Although few of them, there are a lot of fun-to-play features that demonstrate the capabilities of this little engine.

![Screenshot 2024-07-09 150744](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/150c019a-a31a-4ac9-a278-09c91d0a8b9a)
*Main View of the engine*

### Main Architecture

The engine works on a basic architecture using OOP principles and singleton patterns. The Main class runs the game core loop and initializes the main components. The most important parts after that are the **Renderer** and the **EventManager**. 

The Renderer is responsible for properly setting up important OpenGL variables that enable the system to render graphics. It is the main source for selecting which objects to activate as well as how to tune them with the 

The EventManager is responsible for setting up the window component and preparing other parameters that are used by the whole program. In addition, all the input parameters, callbacks and interactions for the camera are set up here. In general, the event manager is 

You will notice in the code that the Renderer and the EventManager work together and share a lot of data. These two are the backbone of the program. For the input, you will need a mouse and a keyboard.

There is also the console debug window outside of the application window, that shows logs of different actions happening during the startup phase or a change at runtime.

### The Controls
The control scheme is simple:
- Hold Left Mouse Button down and move it around to rotate around the world origin
- Use Scroll Wheel to zoom in and out of the origin.

### The Scene

The scene is what spawns and initializes all the items found on the world. It spawns and initializes the camera, the light, the model, the grid and the axis. What's most important is that it contains the main **update()** and **draw()** functions that use Time Integration(dt) in order to process changes in time. The update() function is used to update the camera and light, most importantly their changes in space (position and orientation). The draw() function draws the grid and axis (if activated), the light source, the UI and the model, containing its subsequent meshes with their shaders.

### The Particle System

Update v1.2 introduced a Particle Generator, where the user can play around with different settings and create some cool effects. The Particle Generator utilizes concepts like billboarding and optimized particle management. You can tune properties like velocity, lifetime, scale, color, texture, rotation, range and you can also randomize most of these. Depending on the workstation the program is running on, it can comfortable run 10,000 particles with 60 FPS. This can be achieved due to instancing!

![image](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/2bcc4ee3-37db-49bd-a757-a1fcdaf58055)
*Campfire simulation using the Particle Generator*

To import your own particle image, you can import your own .png 256x256 images into the assets/particles folder. Once you reboot the program, you can see its name in the particles image dropdown and select it.

### The Model Object (and how to import your own)

You can now import your own models into the Bak3D Engine! This was the initial purpose of the engine, where I used it to import my own models and perform a lookdev process. To import your own models and textures, here are the following steps:
- Once exported from a modeling engine, ensure to apply all transformations and ensure the UV coordinates, normals and triangulated meshes are checked
- Export in OBJ format, where a .mtl file will be exported as well
- Ensure in the .mtl file, all the textures you want to include (diffuse, normal, specular, etc) are properly mapped
- Put all textures and files into one folder with the proper model name and place it into the assets directory of the project
- Once engine is booted up, you can now see and select your models from the model selection dropdown!
- Tip: if textures aren't mapped right, it means the textures need to be flipped vertically in a photo editor

![image](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/a098a5e4-e5d3-4fd5-a0a4-3ae09824a952)
*Custom Mushroom model imported into the engine with appropriate textures.*

### Model Render Breakdowns

Under the main GUI panel "Settings", there is a section called *Render Breakdowns*, that toggles which texture and light property is applied to the model. Since there is only Albedo, Specular and Normal maps applied (so far), checking and unchecking each option shows the final result of the model's render.

![Screenshot 2023-01-13 022419](https://user-images.githubusercontent.com/40285461/212521352-fe7fb3d9-fa28-4bf1-a94f-23c69de3f450.png)
*A render slice of the model without albedo and specular maps, but only the normal maps. Notice the default color of the model is light gray.*

### Material Properties

Under the section "Material Properties" in Settings, there are two miscellaneous options that modify the look of the model, using light properties. The options are shininess and gamma correction. One is a slider and another one is a checkbox respectively.

![Screenshot 2023-01-13 022510](https://user-images.githubusercontent.com/40285461/212521500-b6f1af2e-7965-4ccd-85a3-839185d7ba38.png)
*Gamma correction is applied to the model's material properties*

### Texture Slicing

Under the *Map Breakdowns* GUI panel, you will see four checkboxes that correspond to each texture map to show. This will set each of these textures as diffuse with no light properties. This is just to show how the texture is applied to the model in order to produce the final result. To apply this look, the model loads a separate shader that only uses a diffuse texture as a fragment color and no other properties. The four modes are Full Render, Albedo, Specular and Normal. Full Render is the default mode that restores the combined result of all texture processes in the model's main shader.

![Screenshot 2023-01-13 022551](https://user-images.githubusercontent.com/40285461/212521301-8a60ff56-d229-488d-a779-d3dc3f83f148.png)
*A texture breakdown of the specular map, that shows how specular lighting is applied to the model*

### Important Metrics

You will notice at the bottom of the screen, there is a GUI panel named *Metrics*. This panel shows some basic metrics, like frames-per-second and miliseconds per frame. The main components these metrics are based on are for ImGUI I/O, main application and the deltaTime value for the EventManager, which is subsequently used for the update(dt) function. In general, the program should run on 60.0 FPS and an approximate dt average of 16 ms/frame. If the FPS is lower and/or the dt is higher, the program might have some performance issues on your workstation.

In v1.2 a histograph that shows the frame time for the last 90 frames was introduced, but it is currently a little bit broken...it will be fixed in the next update.

![image](https://github.com/George540/bak3d_rendering_demo_engine/assets/40285461/9ef84361-7408-4447-b032-56d7a1905c7d)
*The metrics of the application are displayed*

### Next Features?

**v1.3 - The Abstraction Update** contains some interesting features:
- More Class Abstraction (GameObject, components, inheritance, polymorphism, and more)
- Framebuffers (render, texture, etc)
- SSAO
- Deferred Rendering
- Anti-Aliasing
- Improve editor with DreamGUI (multiple panels)
- Wireframe view of the model

Possibly in **v1.4 - The Animated Universe Update**, the following updates will be made:
- GPU Accelerated Particle System
- Paticle Forces
- Lights menu
- Multiple Objects in scene
- Bonus: Rigidbody Objects??

Most of these I just glossed over their theory just to know what are they like, but once I jump back into this project, these should be top priority. More miscellaneous features can be added that will enhance user experience, such as realtime custom model loading, multiple camera views, inter-adjustable windows with more properties to modify, more metrics and many more. This can easily be turned into a proper CG engine, but I realize it takes more time than anticipated.


## Epilogue

In general, this is a great project with a lot of meaning behind it. Computer graphics has always been the bread and butter of my career and studies. To know what's going on behind a video game or an animated movie in terms of programming, it makes me appreciate even more the craft of making things appear in screen like making games or animated movies. When I look at engines now like Blender, Unity, Unreal, Godot and Maya, I have a different perspective when I use them.

I am currently working as a graphics developer at flight simulators and this was a good project to get me on track with my onboarding at work. I hope you enjoy using my little beginner engine and I hope to learn many more things ahead.

## Acknowledgments

1. Special thanks to [Joe de Vries](http://joeydevries.com/#home) for his ebook ["Lean OpenGL"](https://learnopengl.com/About), the number one source for learning OpenGL online. To attribute his work properly, here is [Joe's twitter](https://twitter.com/JoeyDeVriez.) as well. The code provided on this ebook is licensed under the [CC BY 4.0](https://creativecommons.org/licenses/by-nc/4.0/) license. Proper attribution was given as well as any direct reference of his work in my code. This work is non-commercial.

2. I also appreciate the experts at [Graphics Programming](https://discord.gg/DStdQSaN) and [Together C & C++](https://discord.gg/tccpp) discord servers for answering my basic questions when I needed help understanding something in OpenGL and C++.
