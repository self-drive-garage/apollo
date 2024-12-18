// main.cpp

#include "falconview/Application.hpp"

#include <iostream>

int main()
{
    try
    {
        falconview::Application app("MAIN WINDOW TITLE", 1280, 720);
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application encountered an error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
