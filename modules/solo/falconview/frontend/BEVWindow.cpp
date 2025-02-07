#include "falconview/frontend/BEVWindow.hpp"
#include "falconview/imgui/imgui.h"
#include "falconview/frontend/UIConstants.hpp" // Include constants

namespace falconview::frontend
{
BEVWindow::BEVWindow()
{
}

BEVWindow::~BEVWindow()
{
}

void BEVWindow::render()
{ 
    // Get display size
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 display_size = io.DisplaySize;
    float display_width = display_size.x;
    float display_height = display_size.y;

    // Use shared bottom menu height
    const float bottom_menu_height = BOTTOM_MENU_HEIGHT;

    // Calculate BEVWindow size
    float bev_width = display_width / 3.0f;
    float bev_height = display_height - bottom_menu_height;

    // Position at top-left corner
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(bev_width, bev_height));

    // Window flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoCollapse;

    // Set window background color to white
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 1));

    // Begin the window with the specified flags
    ImGui::Begin("BEVWindow", nullptr, window_flags);

    // Add your content here
    ImGui::Text("BEVWindow Content Goes Here");

    // End the window
    ImGui::End();

    // Restore previous style
    ImGui::PopStyleColor();
}

}   