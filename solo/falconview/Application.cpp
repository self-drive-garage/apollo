

#include "falconview/Application.hpp"
#include "falconview/imgui/imgui_impl_glfw.h"
#include "falconview/imgui/imgui_impl_opengl3.h"

#include <stdexcept>

namespace falconview
{
Application::Application(const std::string& title, int width, int height)
    : parentWindow_(std::make_unique<Window>(title, width, height)),
      showDemoWindow_(true),
      showAnotherWindow_(true),
      clearColor_(0.45f, 0.55f, 0.60f, 1.00f),
      counter_(0)
{
    // Initialize the window
    

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();

    // Setup ImGui configuration flags
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(parentWindow_->getGLFWwindow().get(), true);
    ImGui_ImplOpenGL3_Init(parentWindow_->getGLSLVersion().c_str());

    windows_ = std::make_tuple(
        frontend::BottomMenu(),
        frontend::BEVWindow()
    );
}

Application::~Application()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run()
{
    mainLoop();
}

void Application::mainLoop()
{
    auto& io = ImGui::GetIO();

    while (!parentWindow_->shouldClose())
    {
        parentWindow_->pollEvents();

        if (parentWindow_->isIconified())
        {
            parentWindow_->sleep(std::chrono::milliseconds(10));
            continue;
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the UI
        renderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        parentWindow_->getFramebufferSize(display_w, display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor_.x * clearColor_.w, clearColor_.y * clearColor_.w,
                     clearColor_.z * clearColor_.w, clearColor_.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        parentWindow_->swapBuffers();
    }
}

void Application::renderUI()
{
    std::apply([](auto&... window) { ((window.render()), ...); }, windows_);
}
}