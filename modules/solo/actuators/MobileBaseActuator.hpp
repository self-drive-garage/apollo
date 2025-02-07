#pragma once

#include "platform/actuators/SabertoothMotorController.hpp"
#include "platform/io/Status.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

namespace platform::actuators {

class MobileBaseActuator : public rclcpp::Node
{
public:
    MobileBaseActuator(std::string topicName)
        : Node("MobileBaseActuator", node_options)
    {

    };

    ~MobileBaseActuator() {
        stopMotors();
    }

private:
//    void velocityCallbackStamped(const geometry_msgs::msg::TwistStamped& msg);

    void stopMotors() {
        sabertoothMotorController_.mixedModeDrive(0, 0); // Stop both motors
    }

    void velocityCallback() ;

    void processVelocity(double linear, double angular);

    int queueSize_ = 10;

    static constexpr double maxLinearSpeed_ = 1.0;
    static constexpr double maxAngularSpeed_ = 1.0;
    static constexpr double rearWheelRadius_ = 0.0375;
    static constexpr double rearWheelSeparation_ = 0.13;

    double normalizeToHalfRange(double value) {
        // Clamp the value to be between -1.0 and 1.0
        value = std::clamp(value, -1.0, 1.0);
        // Scale the clamped value to the range [-0.5, 0.5]
        return 0.01 * value;
    }

};

} // namespace platform::actuators
