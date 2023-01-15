# The Bak3D Rendering Demo Engine
Made by George Mavroeidis

## Motivation
I always wanted to improve my fundamentals of computer graphics, so this project is a demonstration of multiple concepts regarding the graphics pipeline. Back in school, I used to study a lot of this subject, but I always felt I was leaving some important knowledge behind as time passed on. So to mitigate that, I thoroughly reviewed computer graphics the past few months by reading and studying [learnopengl.com] (https://learnopengl.com/Introduction)(more on this later).

## What is Bak3D
Bak3D is a demo rendering engine that demonstrates light and texture properties using a model made up of multiple mesh objects. The basis of these properties rely on shaders, vertex arrays and buffer objects. All the content such as models and textures are loaded up using asset importing, a technique that loads generic 3D models with textures. The user is able to also control the camera and position themselves relatively to the center of the world, which is surrounded by a grid with the model in the middle. 

![Screenshot 2023-01-13 022219](https://user-images.githubusercontent.com/40285461/212265740-21610e8f-d382-48f2-a4ae-c02688c97172.png)
*Figure 1: Main View of the engine*

## How it's made
The project is made in C++ and [OpenGL](https://www.opengl.org), two crucial tools that are necessary to use in computer graphics. Additionally, the [Assimp library](https://github.com/assimp/assimp) is used to assist with model loading and [Dear ImGui](https://github.com/ocornut/imgui) for the user interface. As you might have noticed, all these tools are open-source and free to use. More details on the Aknowledgements section.

## How to launch
Download and extract the latest .zip folder on the *Releases* section onto your personal computer. Enter the folder and launch the **Bak3D Engine** .exe application. The program will run in windowed version that covers the entire display.

## Features
Although few of them, there are a lot of fun-to-play features that demonstrate the capabilities of this little engine.

### Main Architecture

The engine works on a basic architecture using OOP principles and singleton patterns. The Main class runs the game core loop and initializes the main components. The most important parts after that are the **Renderer** and the **EventManager**. 

The Renderer is responsible for properly setting up important OpenGL variables that enable the system to render graphics. It also takes care of bufffers, depth tests and the GUI settings. It is the main source for drawing objects in the scene.

The EventManager is responsible for setting up the window component and preparing other parameters that are used by the whole program. In addition, all the input parameters, callbacks and interactions for the camera are set up here. In general, the event manager is 

You will notice in the code that the Renderer and the EventManager work together and share a lot of data. These two are the backbone of the program. For the input, you will need a mouse and a keyboard.

### The Camera

The camera is simple to use but allows for enough freedom to properly view and orient yourself in the scene relative to the model. It utilizes a basic Model-View-Projection approach for a proper perspective view of the world. When holding left click and moving the mouse, the camera will be orbiting around the world origin. With the middle mouse (scrollwheel), the camera can be zoomed in and out in order to get a closer look of the model. The zoom is not infinite and is clamped between two values, enough to get very close and far enough to view everything on the scene. More or less than that is unecessary.

### The Grid
The grid is a 20x20 size grid with 40x40 slices and helps orient the camera and model in space. It also includes a XYZ gizmo axis with RGB colors. The axis is there as a tool of reference for the scene. The grid and axis can be toggled with the *Render Grid* checkbox.

### The Light

One of the most important components in the scene. The light is a simple cube that is used as a source of light emission in the scene. The model's main shader uses the light's position as a reference to process lighting algorithms, such as the **Bling-Phong model**. 

On the GUI window, there is a section called *Light Repositioning*, where it can be repositioned the same way the camera does. The light can orbit around the world origin using the "Horizontal" and "Vertical" sliders. There is also a "Distance" slider that brings the light further or closer to the model. 

![Screenshot 2023-01-13 022309](https://user-images.githubusercontent.com/40285461/212520514-44dd8b08-a631-4b03-8375-cb6e00dc2769.png)
*Figure 2: Light repositioning renders a different lighting look on the model. This is good for operating lookdev on a model*

The intensity of the light as well as its color can also be modified, using the "Intensity" slider and the "Light Color" color picker respectively.

![Screenshot 2023-01-13 022353](https://user-images.githubusercontent.com/40285461/212520539-662b2c6b-cdc6-416f-ac5a-4bcd8081103e.png)
*Figure 3: Change of light color and intensity results in a different mood, enabling artists to try different looks with their models*

### The Scene

The scene is what spawns and initializes all the items found on the world. It spawns and initializes the camera, the light, the model, the grid and the axis. What's most important is that it contains the main **update()** and **draw()** functions that use Time Integration(dt) in order to process changes in time. The update() function is used to update the camera and light, most importantly their changes in space (position and orientation). The draw() function draws the grid and axis (if activated), the light source, the UI and the model, containing its subsequent meshes with their shaders.

### Render Breakdowns

Under the main GUI panel "Settings", there is a section called *Render Breakdowns*, that toggles which texture and light property is applied to the model. Since there is only Albedo, Specular and Normal maps applied (so far), checking and unchecking each option shows the final result of the model's render.

![Screenshot 2023-01-13 022419](https://user-images.githubusercontent.com/40285461/212521352-fe7fb3d9-fa28-4bf1-a94f-23c69de3f450.png)
*Figure 4: A render slice of the model without albedo and specular maps, but only the normal maps. Notice the default color of the model is light gray.*

### Material Properties

Under the section "Material Properties" in Settings, there are two miscellaneous options that modify the look of the model, using light properties. The options are shininess and gamma correction. One is a slider and another one is a checkbox respectively.

![Screenshot 2023-01-13 022510](https://user-images.githubusercontent.com/40285461/212521500-b6f1af2e-7965-4ccd-85a3-839185d7ba38.png)
*Figure 5: Gamma correction is applied to the model's material properties*

### Texture Slicing

Under the *Map Breakdowns* GUI panel, you will see four checkboxes that correspond to each texture map to show. This will set each of these textures as diffuse with no light properties. This is just to show how the texture is applied to the model in order to produce the final result. To apply this look, the model loads a separate shader that only uses a diffuse texture as a fragment color and no other properties. The four modes are Full Render, Albedo, Specular and Normal. Full Render is the default mode that restores the combined result of all texture processes in the model's main shader.

![Screenshot 2023-01-13 022551](https://user-images.githubusercontent.com/40285461/212521301-8a60ff56-d229-488d-a779-d3dc3f83f148.png)
*Figure 5: A texture breakdown of the specular map, that shows how specular lighting is applied to the model*

### Important Metrics

You will notice at the bottom of the screen, there is a GUI panel named *Metrics*. This panel shows some basic metrics, like frames-per-second and miliseconds per frame. The main components these metrics are based on are for ImGUI I/O, main application and the deltaTime value for the EventManager, which is subsequently used for the update(dt) function. In general, the program should run on 60.0 FPS and an approximate dt average of 16 ms/frame. If the FPS is lower and/or the dt is higher, the program might have some performance issues on your workstation.

![image](https://user-images.githubusercontent.com/40285461/212521659-51cb4d9d-617d-4b4a-9f0f-b556a90558ab.png)
*Figure 6: The metrics of the application are displayed*

### Next Features?

I am currently unsure on how far I want to upgrade this simple demo rendering engine, but this was mainly a personal project that was a form of exercise using study material online. But these are the main features I would like to add to enhance my computer graphics skills:
- Framebuffers (render, texture, etc)
- SSAO
- Deferred Rendering
- Anti-Aliasing

Most of these I just glossed over their theory just to know what are they like, but once I jump back into this project, these should be top priority. More miscellaneous features can be added that will enhance user experience, such as realtime custom model loading, multiple camera views, inter-adjustable windows with more properties to modify, more metrics and many more. This can easily be turned into a proper CG engine, but I realize it takes more time than anticipated.


## Epilogue

In general, this was a great project with a lot of meaning behind it. Computer graphics has always been the bread and butter of my career and studies. To know what's going on behind a video game or an animated movie in terms of programming, it makes me appreciate even more the developers that spend countless ours making things like these happen on a screen. When I look at engines now like Blender, Unity and Unreal, I have a different perspective when I use them.

I am currently working as a graphics developer at flight simulators and this was a good project to get me on track with my onboarding at work. I hope you enjoy using my little beginner engine and I hope to learn many more things ahead.

## Acknowledgments

1. Special thanks to [Joe de Vries](http://joeydevries.com/#home) for his ebook ["Lean OpenGL"](https://learnopengl.com/About), the number one source for learning OpenGL online. To attribute his work properly, here is [Joe's twitter](https://twitter.com/JoeyDeVriez.) as well. The code provided on this ebook is licensed under the [CC BY 4.0](https://creativecommons.org/licenses/by-nc/4.0/) license. Proper attribution was given as well as any direct reference of his work in my code. This work is non-commercial.

2. I also appreciate the experts at [Graphics Programming](https://discord.gg/DStdQSaN) and [Together C & C++](https://discord.gg/tccpp) discord servers for answering my basic questions when I needed help understanding something in OpenGL and C++.
