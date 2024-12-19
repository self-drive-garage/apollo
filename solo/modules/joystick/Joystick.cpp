#include "solo/modules/joystick/Joystick.hpp"
#include "modules/common_msgs/control_msgs/control_cmd.pb.h"

#include <cmath>

namespace solo::modules::joystick {
Joystick::Joystick()
    : fd_(-1)
{
}

bool Joystick::Init() {
    std::string device_path = "/dev/input/js0";

    fd_ = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd_ < 0) {
        AERROR << "Failed to open joystick device " << device_path << " " <<
            strerror(errno);
        return false;
    }
    AINFO << "Joystick device" << device_path << "opened successfully";

    // Initialize the publisher for ControlCommand messages
    try {
        AINFO << "ControlCommand publisher initialized successfully.";
    } catch (const std::exception& e) {
        AERROR << "Exception while initializing publisher " << e.what();
        return false;
    }
    return true;
}


Joystick::JoystickControl Joystick::update() {
    js_event event;
    if (read(fd_, &event, sizeof(event)) == sizeof(event)) {
        handleEvent(event);
    }
    return joystickControl_;
}

void Joystick::handleEvent(const js_event& event) {
    if (event.type == JS_EVENT_AXIS) {
        switch (event.number) {
            case 0: // Left stick horizontal
                handleSteering(event);
                break;
            case 4: // Right trigger
                handleThrottle(event);
                break;
            case 5: // Left trigger
                // handleBrake(event);
                break;
        }
    }

    if (event.type == JS_EVENT_BUTTON) {
        if (event.number == 0) {
            // Square button
            joystickControl_.square_button_pressed_ = (event.value == 1);
        }
        if (event.value == 1) {
            handleGearSelection(event);
        }
    }
}

void Joystick::handleSteering(const js_event& event) {
    joystickControl_.steering_angle_ = static_cast<double>(event.value) / 32767.0 *
                      M_PI / 4; // Scale to ±45 degrees
}

void Joystick::handleThrottle(const js_event& event) {
    joystickControl_.throttle_ = static_cast<double>(event.value + 32767) / 65534.0;
    // Scale to 0 to 1
    if (joystickControl_.square_button_pressed_) {
        joystickControl_.throttle_ = -joystickControl_.throttle_;
        // Make throttle negative if square button is pressed
    }
}

void Joystick::handleBrake(const js_event& event) {
    joystickControl_.brake_ = static_cast<double>(event.value + 32767) / 65534.0 * 100.0;
    // Scale to 0-100%
}

void Joystick::handleGearSelection(const js_event& event) {
    switch (event.number) {
        case 1: // Circle button (parking brake)
            joystickControl_.parking_brake_ = !joystickControl_.parking_brake_;
            break;
        case 3: // Square button (gear position)
            if (joystickControl_.gear_position_ == 1) {
                joystickControl_.gear_position_ = 0; // Switch to Neutral
            } else if (joystickControl_.gear_position_ == 0) {
                joystickControl_.gear_position_ = -1; // Switch to Reverse
            } else {
                joystickControl_.gear_position_ = 1; // Switch to Drive
            }
            break;
    }
}
} //solo::modules::joystick