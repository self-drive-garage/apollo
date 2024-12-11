#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <memory>
#include <chrono>

class Window
{
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    void swapBuffers();
    
    void pollEvents();
    
    bool shouldClose() const;
    
    void getFramebufferSize(int& width, int& height) const;
    
    std::shared_ptr<GLFWwindow> getGLFWwindow() const;
    
    const std::string& getGLSLVersion() const;
    
    bool isIconified() const;

    void sleep(std::chrono::milliseconds milliseconds) const;

private:
    void initGLFW();

    void createWindow(const std::string& title, int width, int height);
    
    static void errorCallback(int error, const char* description);

    std::shared_ptr<GLFWwindow> window_;
    const std::string glslVersion_;
};
