// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Framework created by: George Mavroeidis
// 
// Date created on 09/08/2022.
// 
// All dependencies and libraries used listed on README.md and Project Report

#include <iostream>

#include "renderer.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "scene.h"
#include "user_interface.h"

int main()
{
    std::cout << "Hello Computer Graphics!\n";

    EventManager::initialize();
    Renderer::initialize();
    ResourceManager::initialize();
    UserInterface::initialize();

    auto scene = Scene();

    do
    {
        // Update Event Manager - Frame time / input / events processing 
        EventManager::update();

        // Update Scene
        const float dt = EventManager::get_frame_time();
        scene.update(dt);

        scene.draw();
    } while (EventManager::is_exit_requested() == false);

    UserInterface::shutdown();
    Renderer::shutdown();
    EventManager::shutdown();
    ResourceManager::shutdown();

    return 0;
}
