#pragma once

#include "cyber/cyber.h"

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace solo::modules::joystick {

class Joystick {
public:
    struct JoystickControl {
        double steering_angle_ = 0.0;
        double throttle_ = 0.0;
        double brake_ = 0.0;
        bool parking_brake_ = false;
        bool square_button_pressed_ = false;
        short gear_position_ = 1; // 1 = Drive, 0 = Neutral, -1 = Reverse
    };
public:
    Joystick();

    bool Init();

    JoystickControl update();

private:
    void handleEvent(const js_event& event);
    void handleSteering(const js_event& event);
    void handleThrottle(const js_event& event);
    void handleBrake(const js_event& event);
    void handleGearSelection(const js_event& event);

    int fd_;
    JoystickControl joystickControl_;
};
}