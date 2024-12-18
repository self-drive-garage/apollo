#pragma once

#include "falconview/frontend/Window.hpp"
#include "falconview/frontend/BottomMenu.hpp"
#include "falconview/frontend/BEVWindow.hpp"
#include "falconview/imgui/imgui.h"

#include <string>
#include <memory>

namespace falconview
{
class Application
{
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    void run();

private:
    void mainLoop();
    void renderUI();

    std::unique_ptr<Window> parentWindow_;

    std::tuple<frontend::BottomMenu, frontend::BEVWindow> windows_;

    // ImGui state
    bool showDemoWindow_;
    bool showAnotherWindow_;
    ImVec4 clearColor_;
    int counter_;
};

}