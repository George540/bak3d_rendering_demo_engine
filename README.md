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
### Render Slicing
### Material Properties
### Texture Breakdown
### Important Metrics
### Next Features?

## Epilogue
## Acknowledgments

Under construction!!!
