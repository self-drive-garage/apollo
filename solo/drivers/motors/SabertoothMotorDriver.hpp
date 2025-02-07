#pragma once

#include "modules/common/status/status.h"
#include "solo/drivers//serial/BoostSerialDeviceManager.hpp"

#include <boost/asio.hpp>
#include <string>
#include <cmath>

namespace solo::drivers::motors {

using namespace apollo::common;
using namespace solo::drivers::serial;

/// @brief Class for controlling a Sabertooth motor controller via Simplified Serial mode.
class SabertoothMotorDriver {
public:
  /// @brief Constructs a Sabertooth motor controller object.
  /// @param port The serial port to connect to, e.g., "/dev/ttyTHS0".
  /// @param baudRate The baud rate for serial communication, defaults to 9600.
  SabertoothMotorDriver(const std::string& port = "/dev/ttyTHS1",
                        uint32_t baudRate = 9600);

  /// @brief Destroys the Sabertooth motor controller object, closing any open connections.
  ~SabertoothMotorDriver();

  SabertoothMotorDriver(SabertoothMotorDriver&& other) noexcept;

  SabertoothMotorDriver& operator=(SabertoothMotorDriver&& other) noexcept;

  /// @brief Drives the motor using a normalized speed value.
  /// @param normalizedSpeed Normalized speed value between -1.0 (full reverse) and 1.0 (full forward).
  ///        0.0 corresponds to stop.
  /// @return Status of the operation.
  Status driveMotor(double normalizedSpeed);

  /// @brief Stops the motor by sending the neutral (stop) command.
  /// @return Status of the operation.
  Status stopMotor();

private:
  std::string port_;
  ///< The serial port connected to the Sabertooth controller.
  uint32_t baudRate_; ///< Baud rate for serial communication.
  BoostSerialDeviceManager manager_;
  ///< Manages serial device communication.
};
} // namespace solo::drivers::motors