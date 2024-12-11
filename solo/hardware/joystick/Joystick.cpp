
#include "platform/hardware/joystick/Joystick.hpp"

#include "platform/transport/Transport.hpp"

#include <fcntl.h>
#include <linux/joystick.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <numbers>

namespace solo::platform::hardware::joystick {

Joystick::Joystick(const std::string& device_path)
    : fd_(-1)
{
    fd_ = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd_ < 0) {
        AERROR << "Failed to open joystick device {}: {}", device_path, strerror(errno));
        throw std::runtime_error("Failed to open joystick device");
    }
    AINFO << "Joystick device {} opened successfully", device_path);

    // Initialize the publisher for VehicleControlCommand messages
    try {
        publisher_ =
           transport::Transport::CreatePublisher<solo::platform::messages::VehicleControlCommand>("VehicleControlCommand");
        AINFO << "VehicleControlCommand publisher initialized successfully.");
    } catch (const std::exception& e) {
        AERROR << "Exception while initializing publisher: {}", e.what());
        throw;
    }
}

Joystick::~Joystick()
{
    if (fd_ >= 0) {
        close(fd_);
        AINFO << "Joystick device closed");
    }
}

void Joystick::update()
{
    js_event event;
    while (read(fd_, &event, sizeof(event)) == sizeof(event)) {
        handleEvent(event);
    }
}

void Joystick::handleEvent(const js_event& event)
{
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
        if (event.number == 0) { // Square button
            square_button_pressed_ = (event.value == 1);
        }
        if (event.value == 1) {
            handleGearSelection(event);
        }
    }

    // Create and publish the VehicleControlCommand message
    solo::platform::messages::VehicleControlCommand command;
    command.timestamp(std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count());
    command.angular_velocity(steering_angle_);
    command.linear_acceleration(throttle_);
    command.steering_angle(steering_angle_);
    command.throttle(throttle_);
    command.brake(brake_);
    command.parking_brake(parking_brake_);
    command.gear_position(gear_position_);

    try {
        publisher_->publish(command);
        ADEBUG << "Published VehicleControlCommand: steering = {}, throttle = {}, brake = {}",
                     steering_angle_,
                     throttle_,
                     brake_);
    } catch (const std::exception& e) {
        AERROR << "Failed to publish VehicleControlCommand: {}", e.what());
    }
}

void Joystick::handleSteering(const js_event& event)
{
    steering_angle_ = static_cast<double>(event.value) / 32767.0 * std::numbers::pi / 4; // Scale to ±45 degrees
}

void Joystick::handleThrottle(const js_event& event)
{
    throttle_ = static_cast<double>(event.value + 32767) / 65534.0; // Scale to 0 to 1
    if (square_button_pressed_) {
        throttle_ = -throttle_; // Make throttle negative if square button is pressed
    }
}

void Joystick::handleBrake(const js_event& event)
{
    brake_ = static_cast<double>(event.value + 32767) / 65534.0 * 100.0; // Scale to 0-100%
}

void Joystick::handleGearSelection(const js_event& event)
{
    switch (event.number) {
        case 1: // Circle button (parking brake)
            parking_brake_ = !parking_brake_;
            break;
        case 3: // Square button (gear position)
            if (gear_position_ == 1) {
                gear_position_ = 0; // Switch to Neutral
            } else if (gear_position_ == 0) {
                gear_position_ = -1; // Switch to Reverse
            } else {
                gear_position_ = 1; // Switch to Drive
            }
            break;
    }
}

} // namespace solo::platform::hardware::joystick
