#include "solo/modules/drivers/motors/SabertoothMotorDriver.hpp"
#include "cyber/common/log.h"

namespace solo::drivers::motors {
SabertoothMotorDriver::SabertoothMotorDriver(const std::string& port,
                                             uint32_t baudRate)
  : port_(port)
    , baudRate_(baudRate) {
  manager_.createDevice(port, baudRate);
}

SabertoothMotorDriver::~SabertoothMotorDriver() {
  manager_.closeDevice(port_);
}

Status SabertoothMotorDriver::driveMotor(double normalizedSpeed) {
  // Clamp the normalized speed to the valid range [-1.0, 1.0]
  normalizedSpeed = std::clamp(normalizedSpeed, -1.0, 1.0);

  // Convert normalized speed to a Simplified Serial command
  int speedCommand;
  if (std::abs(normalizedSpeed) < std::numeric_limits<double>::epsilon()) {
    speedCommand = 64; // Stop
  } else if (normalizedSpeed > 0) {
    speedCommand = 64 + static_cast<int>(normalizedSpeed * 63);
  } else {
    speedCommand = 64 + static_cast<int>(normalizedSpeed * 63);
    // Negative speed adjusts properly
  }

  AINFO << "Sending speed command: {}" << speedCommand;

  // Send the command to the Sabertooth
  std::string command(1, static_cast<char>(speedCommand));
  return manager_.writeToDevice(port_, std::move(command));
}

Status SabertoothMotorDriver::stopMotor() {
  return driveMotor(0.0); // Stop the motor
}


SabertoothMotorDriver::SabertoothMotorDriver(
    SabertoothMotorDriver&& other) noexcept
  : port_(std::move(other.port_))
    , baudRate_(other.baudRate_)
    , manager_(std::move(other.manager_)) {
  other.baudRate_ = 0; // Reset other to a valid, empty state
}

SabertoothMotorDriver& SabertoothMotorDriver::operator=(
    SabertoothMotorDriver&& other) noexcept {
  if (this != &other) {
    port_ = std::move(other.port_);
    baudRate_ = other.baudRate_;
    manager_ = std::move(other.manager_);
    other.baudRate_ = 0;
  }
  return *this;
}
} // namespace solo::drivers::motors