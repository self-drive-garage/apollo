#pragma once

#include "platform/actuators/SabertoothMotorController.hpp"
#include "platform/actuators/SteeringServoMotorController.hpp"
#include "platform/io/Status.hpp"
#include "platform/messages/VehicleControlCommand.hpp"
#include "platform/transport/TransportSubscriber.hpp"

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

namespace solo::platform::actuators {

using namespace solo::platform::transport;

class VehicleMotionController
{
public:
    VehicleMotionController(std::string topicName,
                            SabertoothMotorController sabertoothMotorController,
                            SteeringServoMotorController steeringServoMotorController);

    ~VehicleMotionController();

private:
    void stopMotors();

    // void vehicleControlCommandCallback();

    void processVelocity(double linear, double angular);

    double normalizeToHalfRange(double value)
    {
        // Clamp the value to be between -1.0 and 1.0
        value = std::clamp(value, -1.0, 1.0);
        // Scale the clamped value to the range [-0.5, 0.5]
        return 0.01 * value;
    }

    int queueSize_ = 10;

    static constexpr double maxLinearSpeed_ = 1.0;
    static constexpr double maxAngularSpeed_ = 1.0;
    static constexpr double rearWheelRadius_ = 0.0375;
    static constexpr double rearWheelSeparation_ = 0.13;

    std::shared_ptr<TransportSubscriber<messages::VehicleControlCommand>> subscriber_;
    SabertoothMotorController sabertoothMotorController_;
    SteeringServoMotorController steeringServoMotorController_;
};

} // namespace solo::platform::actuators
