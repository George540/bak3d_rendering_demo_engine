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

#include "renderdoc_manager.h"

#include <windows.h>

#include "renderdoc/renderdoc_app.h"

#include "Core/logger.h"

namespace
{
    RENDERDOC_Version rdc_version = eRENDERDOC_API_Version_1_4_0;
    RENDERDOC_API_1_4_0* rdc_api  = nullptr;
    int frames_to_wait = 0;
}

void RenderDocManager::initialize()
{
    // Try to find the DLL (will succeed if launched via RenderDoc)
    HMODULE mod = GetModuleHandleA("renderdoc.dll");

    if (!mod)
    {
        // Fallback: Manually load if you want the API available without launching from RD
        mod = LoadLibraryA("C:\\Program Files\\RenderDoc\\renderdoc.dll");
    }

    if (mod)
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");

        if (RENDERDOC_GetAPI)
        {
            // SUCCESS CHECK: ret == 1 means the pointer was successfully populated
            const int ret = RENDERDOC_GetAPI(rdc_version, (void**)&rdc_api);
            
            if (ret == 1) 
            {
                B3D_LOG_INFO("RenderDoc API v1.4.0 initialized successfully.");
            }
            else
            {
                rdc_api = nullptr;
                B3D_LOG_WARNING("RenderDoc API failed to initialize. Error code: %d.", ret);
            }
        }
    }
    else
    {
        // This is normal when running the app for regular development
        B3D_LOG_INFO("RenderDoc not detected. In-app capture features disabled.");
    }
}

void RenderDocManager::update()
{
    if (frames_to_wait > 0)
    {
        frames_to_wait--;

        if (frames_to_wait == 0)
        {
            B3D_LOG_INFO("Frame captured. Opening RenderDoc UI...");
            launch_renderdoc_ui();
        }
    }
}

void RenderDocManager::shutdown()
{
    delete rdc_api;
}

void RenderDocManager::trigger_capture()
{
    if (rdc_api != nullptr)
    {
        rdc_api->TriggerCapture();
        B3D_LOG_INFO("Capturing Frame. Opening RenderDoc UI...");

        // Wait 2 frames to give enough time for the UI to launch: 
        // Frame 1: The frame being recorded.
        // Frame 2: The frame where the file is finalized, and UI is launched.
        frames_to_wait = 2; 
    }
    else
    {
        B3D_LOG_WARNING("RenderDoc UI not initialized. Aborting capture.");
    }
}

void RenderDocManager::launch_renderdoc_ui()
{
    if (rdc_api != nullptr)
    {
        // Make RenderDoc UI visible if already running
        if (rdc_api->IsTargetControlConnected())
        {
            B3D_LOG_INFO("Bringing RenderDoc to foreground...");
            rdc_api->ShowReplayUI();
        }
        else
        {
            B3D_LOG_INFO("Launching RenderDoc...");
            // Return the PID of the replay UI if successful, 0 if not.
            if (rdc_api->LaunchReplayUI(1, "") == 0)
            {
                B3D_LOG_WARNING("Failed to launch RenderDoc due to miss-matching PID. Aborting launch.");
            }
        }
    }
    else
    {
        B3D_LOG_WARNING("Failed to launch RenderDoc due to missing API reference. Aborting launch.");
    }
}
