#include "solo/joystick/JoystickComponent.hpp"
#include "modules/common_msgs/control_msgs/control_cmd.pb.h"

#include <cmath>

namespace solo::joystick {
JoystickComponent::JoystickComponent()
  : fd_(-1) {
}

bool JoystickComponent::Init() {
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
    node_->CreateWriter<ControlCommand>("VehicleControlCommand");
    AINFO << "ControlCommand publisher initialized successfully.";
  } catch (const std::exception& e) {
    AERROR << "Exception while initializing publisher " << e.what();
    return false;
  }
  return true;
}


bool JoystickComponent::Proc(const std::shared_ptr<ControlCommand>& msg) {
  js_event event;
  while (read(fd_, &event, sizeof(event)) == sizeof(event)) {
    handleEvent(event);
  }
  return true;
}

void JoystickComponent::handleEvent(const js_event& event) {
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
      square_button_pressed_ = (event.value == 1);
    }
    if (event.value == 1) {
      handleGearSelection(event);
    }
  }

  try {
    std::shared_ptr<ControlCommand> command_message(new(std::nothrow)
        ControlCommand);

    command_message->set_acceleration(throttle_);
    command_message->set_steering_rate(steering_angle_);
    command_message->set_throttle(throttle_);
    command_message->set_brake(brake_);
    command_message->set_parking_brake(parking_brake_);
    // command_message->set_gear_location(gear_position_);

    ADEBUG <<
        " Published VehicleControlCommand: " <<
        " Steering " << steering_angle_ <<
        " Throttle " << throttle_ <<
        " Brake " << brake_;
  } catch (const std::exception& e) {
    AERROR << "Failed to publish VehicleControlCommand: " << e.what();
  }
}

void JoystickComponent::handleSteering(const js_event& event) {
  steering_angle_ = static_cast<double>(event.value) / 32767.0 *
                    M_PI / 4; // Scale to ±45 degrees
}

void JoystickComponent::handleThrottle(const js_event& event) {
  throttle_ = static_cast<double>(event.value + 32767) / 65534.0;
  // Scale to 0 to 1
  if (square_button_pressed_) {
    throttle_ = -throttle_;
    // Make throttle negative if square button is pressed
  }
}

void JoystickComponent::handleBrake(const js_event& event) {
  brake_ = static_cast<double>(event.value + 32767) / 65534.0 * 100.0;
  // Scale to 0-100%
}

void JoystickComponent::handleGearSelection(const js_event& event) {
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
} //solo::joystick