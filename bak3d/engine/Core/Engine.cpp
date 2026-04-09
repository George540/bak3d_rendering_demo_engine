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

#include "engine.h"

#include <iostream>

#include "editor.h"
#include "global_settings.h"
#include "logger.h"

#include "Asset/resource_manager.h"
#include "Input/event_manager.h"
#include "Renderer/renderer.h"
#include "Scene/scene.h"

/*
 * Main scene object that has full scope of the application.
 */
static Scene* scene = nullptr;

void Bak3DEngine::Initialize()
{
    Logger::initialize();
    GlobalSettings::initialize();
    EventManager::initialize();
    Renderer::initialize();
    ResourceManager::initialize();
    Bak3DEditor::initialize();

    scene = new Scene();

    B3D_LOG_INFO("Engine initialized.");
}

void Bak3DEngine::Update()
{
    do
    {
        // Update Event Manager - Frame time / input / events processing 
        EventManager::update();

        // Update Scene
        const float dt = EventManager::get_frame_time();
        scene->update(dt);

        Renderer::begin_frame();
        Renderer::render_frame();
        Renderer::end_frame();

        Bak3DEditor::update();

        EventManager::end_update();
    } while (EventManager::is_exit_requested() == false);
}

void Bak3DEngine::Shutdown()
{
    Bak3DEditor::shutdown();
    Renderer::shutdown();
    EventManager::shutdown();
    ResourceManager::shutdown();
    GlobalSettings::shutdown();
    Logger::shutdown();
}
