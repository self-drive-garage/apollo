#pragma once

#include "platform/messages/VehicleControlCommand.hpp"
#include "platform/transport/TransportPublisher.hpp"

#include <fcntl.h>
#include <linux/joystick.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace solo::platform::hardware::joystick {

class Joystick
{
public:
    explicit Joystick(const std::string& device_path);
    ~Joystick();

    void update();

    Joystick(const Joystick&) = delete;
    Joystick& operator=(const Joystick&) = delete;

private:
    void handleEvent(const js_event& event);
    void handleSteering(const js_event& event);
    void handleThrottle(const js_event& event);
    void handleBrake(const js_event& event);
    void handleGearSelection(const js_event& event);

    int fd_;
    double steering_angle_ = 0.0;
    double throttle_ = 0.0;
    double brake_ = 0.0;
    bool parking_brake_ = false;
    bool square_button_pressed_ = false;
    short gear_position_ = 1; // 1 = Drive, 0 = Neutral, -1 = Reverse

    std::shared_ptr<transport::TransportPublisher<solo::platform::messages::VehicleControlCommand>> publisher_;
};

} // namespace solo::platform::hardware::joystick
