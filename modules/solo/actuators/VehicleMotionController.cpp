#include "platform/actuators/VehicleMotionController.hpp"

#include "platform/actuators/SabertoothMotorController.hpp"
#include "platform/actuators/SteeringServoMotorController.hpp"
#include "platform/messages/VehicleControlCommand.hpp"
#include "platform/transport/Transport.hpp"
#include "platform/transport/TransportSubscriber.hpp"

#include <spdlog/spdlog.h>

#include <utility>

namespace solo::platform::actuators {

VehicleMotionController::VehicleMotionController(std::string topicName,
                                                 SabertoothMotorController sabertoothMotorController,
                                                 SteeringServoMotorController steeringServoMotorController)
    : sabertoothMotorController_{std::move(sabertoothMotorController)}
    , steeringServoMotorController_{std::move(steeringServoMotorController)}
{
    AINFO << "Initializing VehicleMotionController...");

    try {
        // Create a subscriber for VehicleControlCommand messages
        subscriber_ = Transport::CreateSubscriber<messages::VehicleControlCommand>(
            topicName, [&](const messages::VehicleControlCommand& command) {
                ADEBUG << "Received VehicleControlCommand:");
                ADEBUG << "  Timestamp: {}", command.timestamp());
                ADEBUG << "  Linear Acceleration: {}", command.linear_acceleration());
                ADEBUG << "  Angular Velocity: {}", command.angular_velocity());
                // Add additional fields if needed
                processVelocity(command.linear_acceleration(), command.angular_velocity());
            });

        AINFO << "Successfully subscribed to topic '{}'.", topicName);

    } catch (const std::exception& e) {
        AERROR << "Exception during VehicleMotionController initialization: {}", e.what());
        throw; // Re-throw the exception after logging it
    } catch (...) {
        AERROR << "Unknown error occurred during VehicleMotionController initialization.");
        throw; // Re-throw the exception after logging it
    }

    AINFO << "VehicleMotionController initialized successfully.");
}

VehicleMotionController::~VehicleMotionController()
{
    AINFO << "VehicleMotionController destructor called. Cleaning up resources...");

    try {
        // Stop the motors to ensure a safe shutdown
        stopMotors();
        AINFO << "Motors stopped successfully.");

        // Reset the subscriber to release its resources
        if (subscriber_) {
            subscriber_.reset();
            AINFO << "TransportSubscriber resources released.");
        }

    } catch (const std::exception& e) {
        AERROR << "Exception during VehicleMotionController cleanup: {}", e.what());
    } catch (...) {
        AERROR << "Unknown exception occurred during VehicleMotionController cleanup.");
    }

    AINFO << "VehicleMotionController cleanup completed.");
}

void VehicleMotionController::stopMotors()
{
    sabertoothMotorController_.stopMotor();
    steeringServoMotorController_.stopAll();
}

void VehicleMotionController::processVelocity(double linear, double angular)
{
    AINFO << "Processing velocity for Ackermann drive: linear = {}, angular = {}", linear, angular);

    try {
        // Normalize the linear and angular velocities to their respective maximums
        const double normalizedLinear = std::clamp(linear, -maxLinearSpeed_, maxLinearSpeed_) / maxLinearSpeed_;
        const double normalizedAngular = std::clamp(angular, -maxAngularSpeed_, maxAngularSpeed_) / maxAngularSpeed_;

        AINFO << "Normalized linear velocity: {}, normalized angular velocity: {}", normalizedLinear, normalizedAngular);

        // Send normalized linear velocity to the motor controller
        if (auto motorStatus = sabertoothMotorController_.driveMotor(normalizedLinear); !motorstatus.ok()) {
            AERROR << "Failed to set motor speed: {}", motorStatus.toString());
        }

        // Convert angular velocity to servo angle (radians)
        double steeringAngle = normalizedAngular * (M_PI / 2); // Assume max steering angle is ±45 degrees
        AINFO << "Calculated steering angle (radians): {}", steeringAngle);

        // Send command to the steering servo motor controller
        steeringServoMotorController_.setServoAngleRadians(steeringAngle, 0);

    } catch (const std::exception& e) {
        AERROR << "Exception while processing velocity: {}", e.what());
    } catch (...) {
        AERROR << "Unknown error occurred while processing velocity.");
    }
}


} // namespace solo::platform::actuators
