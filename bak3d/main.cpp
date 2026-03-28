/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#include <iostream>

#include "user_interface.h"
#include "Input/event_manager.h"
#include "Loader/resource_manager.h"
#include "Renderer/renderer.h"
#include "Scene/scene.h"

/*
 * This is the 'main' function. Program execution begins and ends there.
 *
 * Date created on 09/08/2022.
 *
 * All dependencies and libraries used listed on README.md and root CMakeLists.txt
 */
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
