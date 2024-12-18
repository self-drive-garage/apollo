#pragma once

#include "solo/modules/drivers/motors/PCA9685Config.hpp"

#include <unistd.h>

#include <cstdint>
#include <thread>
#include <utility>

namespace solo::drivers::motors {
class SteeringServoMotorController {
public:
  SteeringServoMotorController(const PCA9685Config& config);

  bool begin();

  void reset();

  void setPWMFreq(double freq);

  void setPWM(uint8_t channel, uint16_t on, uint16_t off) const;

  void setServoAngleRadians(double radians, uint8_t channel = 0) const;

  void stopAll() const;

  // Delete copy and move assignment operators
  SteeringServoMotorController(const SteeringServoMotorController&) = delete;
  SteeringServoMotorController& operator=(const SteeringServoMotorController&)
  = delete;

  // Allow moves (no reassignment of config_ needed)
  SteeringServoMotorController(SteeringServoMotorController&& other) noexcept
    : config_(other.config_)
      , i2c_fd{std::exchange(other.i2c_fd, -1)} {
  }

  /// @brief Destructor that ensures proper cleanup of resources.
  ~SteeringServoMotorController() {
    if (i2c_fd >= 0) {
      close(i2c_fd);
    }
  }

private:
  void writeRegister(uint8_t reg, uint8_t value) const;

  uint8_t readRegister(uint8_t reg) const;

  const PCA9685Config config_;
  int i2c_fd = -1;
};
} // namespace solo::drivers::motors