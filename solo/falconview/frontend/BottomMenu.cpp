#include "falconview/frontend/BottomMenu.hpp"
#include "falconview/imgui/imgui.h"
#include "falconview/frontend/UIConstants.hpp" // Include constants

namespace falconview::frontend
{
BottomMenu::BottomMenu()
{
}

BottomMenu::~BottomMenu()
{
}


void BottomMenu::render()
{
    // Remove window decorations and prevent interactions
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoScrollbar |
                                    ImGuiWindowFlags_NoScrollWithMouse;

    // Set window background color to black
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));

    // Get display size
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_size = io.DisplaySize;
    float window_width = window_size.x;
    float window_height = window_size.y;

    // Use shared constants for height
    const float desired_height = BOTTOM_MENU_HEIGHT;

    // Position window at bottom
    float y_pos = window_height - desired_height;

    // Set window position and size
    ImGui::SetNextWindowPos(ImVec2(0, y_pos));
    ImGui::SetNextWindowSize(ImVec2(window_width, desired_height));

    // Begin the window
    ImGui::Begin("BottomMenu", nullptr, window_flags);

    // Calculate button sizes and positions
    const int num_buttons = 3;
    const float button_width = 120.0f;
    const float total_button_width = num_buttons * button_width + (num_buttons - 1) * BUTTON_SPACING;

    // Center buttons horizontally
    float start_x = (window_width - total_button_width) / 2.0f;
    ImGui::SetCursorPosX(start_x);
    ImGui::SetCursorPosY(BUTTON_SPACING); // Top spacing

    // Render buttons
    if (ImGui::Button("Start", ImVec2(button_width, BUTTON_HEIGHT)))
    {
        // Start action
    }
    ImGui::SameLine(0.0f, BUTTON_SPACING);

    if (ImGui::Button("Stop", ImVec2(button_width, BUTTON_HEIGHT)))
    {
        // Stop action
    }
    ImGui::SameLine(0.0f, BUTTON_SPACING);

    if (ImGui::Button("Calibrate", ImVec2(button_width, BUTTON_HEIGHT)))
    {
        // Calibrate action
    }

    ImGui::End();

    // Restore style
    ImGui::PopStyleColor();
}


} // namespace falconview::frontend

