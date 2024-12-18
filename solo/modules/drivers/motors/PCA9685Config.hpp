#pragma once

#include <string>

namespace solo::drivers::motors {
struct PCA9685Config {
  const std::string I2C_DEVICE = "/dev/i2c-7"; // I2C bus device
  const int PCA9685_ADDR = 0x40; // Default I2C address for PCA9685

  // PCA9685 Registers
  const int MODE1 = 0x00;
  const int PRESCALE = 0xFE;
  const int LED0_ON_L = 0x06;


  // Servo PWM range
  const int SERVOMIN = 200; // FULLY LEFT Minimum pulse length count
  const int SERVOMAX = 500; // FULLY RIGHT Maximum pulse length count
  // const int SERVOCENTER = (SERVOMIN + SERVOMAX) / 2;
  const int SERVOCENTER = 335;
};
} // namespace platform::hardware::motors