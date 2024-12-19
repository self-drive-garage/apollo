#pragma once

#include "cyber/cyber.h"
#include "modules/common_msgs/control_msgs/control_cmd.pb.h"
#include "modules/guardian/guardian_component.h"
#include "solo/modules/drivers/motors/SabertoothMotorDriver.hpp"
#include "solo/modules/drivers/motors/SteeringServoMotorController.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace solo::modules::control {

using apollo::control::ControlCommand;
using apollo::cyber::Component;
using solo::drivers::motors::SabertoothMotorDriver;
using solo::drivers::motors::SteeringServoMotorController;

class AckermanRobotKitComponent : public Component<ControlCommand>
{
public:
    bool Init() override;

    bool Proc(const std::shared_ptr<ControlCommand>& msg) override;

private:
    static constexpr double maxLinearSpeed_ = 1.0;
    static constexpr double maxAngularSpeed_ = 1.0;
    static constexpr double rearWheelRadius_ = 0.0375;
    static constexpr double rearWheelSeparation_ = 0.13;

    std::shared_ptr<apollo::cyber::Reader<ControlCommand>> reader_;
    SabertoothMotorDriver sabertoothMotorController_;
    SteeringServoMotorController steeringServoMotorController_;
};
} // namespace solo::modules::control
