#pragma once

#include "modules/common_msgs/control_msgs/control_cmd.pb.h"
#include "cyber/cyber.h"

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "modules/guardian/guardian_component.h"

namespace solo::modules::joystick {
using apollo::cyber::Component;
using apollo::control::ControlCommand;

class JoystickComponent : public Component<ControlCommand> {
public:
  JoystickComponent();

  bool Init() override;

  void update();

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

  std::shared_ptr<apollo::cyber::Writer<ControlCommand>> writer_;
};
}