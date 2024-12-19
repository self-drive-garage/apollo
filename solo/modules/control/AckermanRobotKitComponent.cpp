#include "solo/modules/control/AckermanRobotKitComponent.hpp"

namespace solo::modules::control {

bool AckermanRobotKitComponent::Init()
{
    reader_ = node_->CreateReader<control::ControlCommand>("VehicleControlCommand");
    return true;
}

bool AckermanRobotKitComponent::Proc(const std::shared_ptr<ControlCommand>& command_message)
{

    //    command_message->set_acceleration(throttle_);
    //    command_message->set_steering_rate(steering_angle_);
    //    command_message->set_throttle(throttle_);
    //    command_message->set_brake(brake_);
    //    command_message->set_parking_brake(parking_brake_);

    auto linear = command_message->acceleration();
    auto angular = command_message->steering_rate();


    AINFO << "Processing velocity for Ackermann drive: linear: " << linear << " angular:" << angular;

    try {
        // Normalize the linear and angular velocities to their respective maximums
        const double normalizedLinear = std::clamp(linear, -maxLinearSpeed_, maxLinearSpeed_) / maxLinearSpeed_;
        const double normalizedAngular = std::clamp(angular, -maxAngularSpeed_, maxAngularSpeed_) / maxAngularSpeed_;

        AINFO << "Normalized linear velocity: " << normalizedLinear << "normalized: " << normalizedLinear
              << " angular velocity: " << normalizedAngular;

        // Send normalized linear velocity to the motor controller
        if (auto motorStatus = sabertoothMotorController_.driveMotor(normalizedLinear); !motorStatus.ok()) {
            AERROR << "Failed to set motor speed: {}", std::to_string(motorStatus.code());
        }

        // Convert angular velocity to servo angle (radians)
        double steeringAngle = normalizedAngular * (M_PI / 2); // Assume max steering angle is ±45 degrees
        AINFO << "Calculated steering angle (radians): " << steeringAngle;

        // Send command to the steering servo motor controller
        steeringServoMotorController_.setServoAngleRadians(steeringAngle, 0);

    } catch (const std::exception& e) {
        AERROR << "Exception while processing velocity: " << e.what();
    } catch (...) {
        AERROR << "Unknown error occurred while processing velocity.";
    }
    return true;
}

} // namespace solo::modules::control
