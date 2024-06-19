// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
    std::cout << "Hello Computer Graphics!\n";

    EventManager::initialize();
    Renderer::initialize();

    auto world = World();

    do
    {
        // Update Event Manager - Frame time / input / events processing 
        EventManager::update();

        // Update World
        const float dt = EventManager::get_frame_time();
        world.process_object_activation();
        world.update(dt);

        world.draw();
    } while (EventManager::is_exit_requested() == false);

    Renderer::shutdown();
    world.delete_arrays_and_buffers();
    EventManager::shutdown();

    return 0;
}