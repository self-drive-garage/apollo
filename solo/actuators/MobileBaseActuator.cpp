#include "platform/actuators/MobileBaseActuator.hpp"

void platform::actuators::MobileBaseActuator::velocityCallback()
{
//    processVelocity(msg.linear.x, msg.angular.z);
}

void platform::actuators::MobileBaseActuator::processVelocity(double linear, double angular) {


    // Map linear and angular speeds directly to motor controller values
    int speed = static_cast<int>((linear / maxLinearSpeed_) * 127);
    int turn = static_cast<int>((angular / maxAngularSpeed_) * 127);

    // Ensure values are within the valid range
    speed = std::clamp(speed, -127, 127);
    turn = std::clamp(turn, -127, 127);

    // Send the values to the motor controller
    sabertoothMotorController_.mixedModeDrive(speed, turn);
}
