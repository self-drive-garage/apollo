#include "solo/modules/drivers/motors/SteeringServoMotorController.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

namespace solo::drivers::motors {
SteeringServoMotorController::SteeringServoMotorController(
    const PCA9685Config& config)
  : config_(config) {
  begin();
  setPWMFreq(60); // Set frequency to 60 Hz for servos
}

bool SteeringServoMotorController::begin() {
  // std::cerr << "Opening I2C device: " << config_.I2C_DEVICE << "\n";
  // i2c_fd = open(config_.I2C_DEVICE.c_str(), O_RDWR);
  // if (i2c_fd < 0) {
  //     perror("Error opening I2C device");
  //     return false;
  // }
  // std::cerr << "Setting I2C_SLAVE address: 0x" << std::hex << config_.PCA9685_ADDR << "\n";
  // if (ioctl(i2c_fd, I2C_SLAVE, config_.PCA9685_ADDR) < 0) {
  //     perror("Error setting I2C_SLAVE address");
  //     close(i2c_fd);
  //     return false;
  // }
  // reset();
  // return true;

  std::cerr << "Opening I2C device: " << config_.I2C_DEVICE << "\n";
  i2c_fd = open(config_.I2C_DEVICE.c_str(), O_RDWR);
  if (i2c_fd < 0) {
    perror("Error opening I2C device");
    return false;
  }
  std::cerr << "Setting I2C_SLAVE address: 0x" << std::hex << config_.
      PCA9685_ADDR << "\n";
  if (ioctl(i2c_fd, I2C_SLAVE, config_.PCA9685_ADDR) < 0) {
    perror("Error setting I2C_SLAVE address");
    close(i2c_fd);
    return false;
  }
  reset();
  return true;
}

void SteeringServoMotorController::reset() {
  uint8_t mode1 = readRegister(config_.MODE1);
  mode1 &= ~0x10; // Clear the SLEEP bit
  writeRegister(config_.MODE1, mode1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  // Allow oscillator to stabilize
}

void SteeringServoMotorController::setPWMFreq(const double freq) {
  std::cerr << "Setting PWM frequency to " << freq << " Hz\n";
  double prescale_val = 25000000.0f / (4096 * freq) - 1;
  uint8_t prescale = static_cast<uint8_t>(std::floor(prescale_val + 0.5f));
  std::cerr << "Calculated prescale: " << static_cast<int>(prescale) << "\n";

  uint8_t oldmode = readRegister(config_.MODE1);
  uint8_t newmode = (oldmode & 0x7F) | 0x10; // Sleep
  writeRegister(config_.MODE1, newmode); // Enter sleep mode
  writeRegister(config_.PRESCALE, prescale); // Set prescaler
  writeRegister(config_.MODE1, oldmode); // Wake up
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  writeRegister(config_.MODE1, oldmode | 0x80); // Restart
  std::cerr << "PWM frequency set.\n";
}

void SteeringServoMotorController::setPWM(uint8_t channel, uint16_t on,
                                          uint16_t off) const {
  writeRegister(config_.LED0_ON_L + 4 * channel, on & 0xFF);
  writeRegister(config_.LED0_ON_L + 4 * channel + 1, on >> 8);
  writeRegister(config_.LED0_ON_L + 4 * channel + 2, off & 0xFF);
  writeRegister(config_.LED0_ON_L + 4 * channel + 3, off >> 8);
}

void SteeringServoMotorController::setServoAngleRadians(
    double radians, const uint8_t channel) const {
  radians = std::clamp(radians, -M_PI / 2, M_PI / 2);
  // Clamp angle to -π/2 to π/2 radians
  int pulse = config_.SERVOCENTER + static_cast<int>(
                (radians / (M_PI / 2)) * (
                  (config_.SERVOMAX - config_.SERVOMIN) / 2));
  setPWM(channel, 0, pulse);
}


void SteeringServoMotorController::stopAll() const {
  for (uint8_t channel = 0; channel < 16; ++channel) {
    setPWM(channel, 0, 0); // Stops PWM signal for all channels
  }
}

void SteeringServoMotorController::writeRegister(
    const uint8_t reg, const uint8_t value) const {
  uint8_t buffer[2] = {reg, value};
  if (write(i2c_fd, buffer, 2) != 2) {
    perror("Failed to write to I2C bus");
  }
}

uint8_t SteeringServoMotorController::readRegister(const uint8_t reg) const {
  if (write(i2c_fd, &reg, 1) != 1) {
    perror("Failed to write register address");
    return 0;
  }
  uint8_t value;
  if (read(i2c_fd, &value, 1) != 1) {
    perror("Failed to read from I2C bus");
    return 0;
  }
  return value;
}
} // namespace solo::actuators