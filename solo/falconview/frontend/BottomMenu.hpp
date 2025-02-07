#pragma once

#include "falconview/imgui/imgui.h"
#include "falconview/frontend/UIConstants.hpp" // Include constants


namespace falconview::frontend
{
class BottomMenu
    {
    public:

        BottomMenu();
    
        ~BottomMenu();

        void render();
    };
}
