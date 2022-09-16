// 3D Model Viewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Framework created by: George Mavroeidis
// 
// Date created on on 09/08/2022.
// 
// All dependencies and libraries used listed on README.md and Project Report

#include <iostream>

#include "renderer.h"
#include "src/event_manager.h"
#include "scene.h"

int main()
{
    std::cout << "Hello World!\n";

    EventManager::initialize();
    Renderer::initialize();

    auto world = World();

    do
    {
        // Update Event Manager - Frame time / input / events processing 
        EventManager::update();

        // Update World
        const float dt = EventManager::get_frame_time();
        world.update(dt);

        world.draw();
    } while (EventManager::is_exit_requested() == false);

    Renderer::shutdown();
    EventManager::shutdown();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
