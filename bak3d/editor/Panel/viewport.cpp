#include "viewport.h"

#include <imgui_internal.h>
#include <glm/common.hpp>

#include "Input/event_manager.h"
#include "Renderer/renderer.h"

namespace
{
    ImVec2 previous_viewport_size = ImVec2(0, 0);
}

Viewport::Viewport() : EditorPanel("Viewport")
{
    m_flags |= ImGuiWindowFlags_NoScrollbar;
}

void Viewport::begin_frame()
{
    EditorPanel::begin_frame();
}

void Viewport::update()
{
    EditorPanel::update();

    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	
    auto frame_buffer_main = Renderer::get_frame_buffer();
    float fb_aspect = frame_buffer_main->get_aspect_ratio();
    float view_aspect = viewport_panel_size.x / viewport_panel_size.y;

    // Start with Y flipped: top is 1.0f, bottom is 0.0f
    ImVec2 uv0(0.0f, 1.0f);
    ImVec2 uv1(1.0f, 0.0f);

    if (view_aspect > fb_aspect)
    {
        // Viewport is wider than the image: Crop the top and bottom
        float scale = fb_aspect / view_aspect;
        float delta = (1.0f - scale) * 0.5f;

        // Offset from the flipped baseline
        uv0.y = 1.0f - delta; // Start slightly below the top (1.0)
        uv1.y = delta;        // End slightly above the bottom (0.0)
    }
    else
    {
        // Viewport is taller than the image: Crop the sides instead
        float scale = view_aspect / fb_aspect;
        float delta = (1.0f - scale) * 0.5f;

        uv0.x = delta;
        uv1.x = 1.0f - delta;
    }

    // Track whether the drag originated from the viewport window
    static bool drag_started_in_viewport = false;

    bool is_window_resizing = ImGui::IsMouseDragging(ImGuiMouseButton_Left)
                              && (glm::abs(previous_viewport_size.x - viewport_panel_size.x) > 0.001f 
                               || glm::abs(previous_viewport_size.y - viewport_panel_size.y) > 0.001f);

    bool is_hovered_over_viewport = ImGui::GetCurrentWindow()->Name == m_title;

    // Latch: only set true when the click begins in the viewport
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_hovered_over_viewport)
    {
        drag_started_in_viewport = true;
    }

    // Release the latch when mouse is released
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        drag_started_in_viewport = false;
    }

    EventManager::is_viewport_active = drag_started_in_viewport && !is_window_resizing;

    void* viewport_texture = reinterpret_cast<void*>(static_cast<intptr_t>(frame_buffer_main->get_render_buffer()));
    ImGui::Image(viewport_texture, viewport_panel_size, uv0, uv1);

    previous_viewport_size = viewport_panel_size;

    bool isHovered = ImGui::IsItemHovered();
}

void Viewport::end_frame()
{
    EditorPanel::end_frame();
}
