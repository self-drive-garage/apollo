

#include "falconview/frontend/Window.hpp"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

Window::Window(const std::string& title, int width, int height)
    : window_(nullptr), glslVersion_("#version 130")
{
    initGLFW();
    createWindow(title, width, height);
}

Window::~Window()
{
    if (window_)
    {
        glfwDestroyWindow(window_.get());
    }
    glfwTerminate();
}

void Window::initGLFW()
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

void Window::createWindow(const std::string& title, int width, int height)
{
    // GL 3.2 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    window_ = std::shared_ptr<GLFWwindow>(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr), glfwDestroyWindow);
    if (!window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_.get());
    glfwSwapInterval(1); // Enable vsync
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window_.get());
}

void Window::pollEvents()
{
    glfwPollEvents();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window_.get());
}

void Window::getFramebufferSize(int& width, int& height) const
{
    glfwGetFramebufferSize(window_.get(), &width, &height);
}

std::shared_ptr<GLFWwindow> Window::getGLFWwindow() const
{
    return window_;
}

const std::string& Window::getGLSLVersion() const
{
    return glslVersion_;
}

bool Window::isIconified() const
{
    return glfwGetWindowAttrib(window_.get(), GLFW_ICONIFIED);
}

void Window::sleep(std::chrono::milliseconds milliseconds) const
{
    std::this_thread::sleep_for(milliseconds);
}

void Window::errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
