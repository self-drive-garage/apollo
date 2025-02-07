#include "solo/modules/drivers/serial/BoostSerialDeviceManager.hpp"

#include <mutex>
#include <string>
#include <tuple>

#include "solo/modules/drivers/serial/BoostSerialDevice.hpp"

#include "cyber/common/log.h"
#include "modules/common/status/status.h"

namespace solo::drivers::serial {
Status BoostSerialDeviceManager::initializeDevice(const std::string& port) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  if (deviceMap_.find(port) != deviceMap_.end() &&
      deviceMap_[port]->isInitialized()) {
    ADEBUG << "Initializing device.";
    return deviceMap_[port]->initialize();
  }
  AINFO << "Device already initialized or not created.";
  return Status::OK(); // Already initialized or device not created
}

Status BoostSerialDeviceManager::openDevice(const std::string& port) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  if (deviceMap_.find(port) != deviceMap_.end() &&
      deviceMap_[port]->isOpened()) {
    ADEBUG << "Opening device.";
    return deviceMap_[port]->open();
  }
  AWARN << "Device already open or not created.";
  // return {STATUS::ERROR, ERROR::NONE}; // Device already open or not created
  return Status(ErrorCode::CONTROL_ERROR, "Device already opened.");
}

Status BoostSerialDeviceManager::closeDevice(const std::string& port) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  if (deviceMap_.find(port) != deviceMap_.end() &&
      deviceMap_[port]->isOpened()) {
    ADEBUG << "Closing device.";
    return deviceMap_[port]->close();
  }
  AINFO << "Device was not open or not created.";
  return Status::OK(); // Device was not open or not created
}

std::tuple<Status, std::optional<std::string>>
BoostSerialDeviceManager::readFromDevice(const std::string& port,
                                         const std::string& request) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  if (deviceMap_.find(port) == deviceMap_.end()) {
    AERROR << "Device not created, cannot read.";
    // return {Status(STATUS::ERROR, ERROR::COMMUNICATION_ERROR), {}};
    return {
        Status(ErrorCode::CONTROL_ERROR, std::string("COMMUNICATION_ERROR")),
        {}};
  }
  ADEBUG << "Reading from device: " << request;
  return deviceMap_[port]->read(request);
}

Status BoostSerialDeviceManager::createDevice(const std::string& port,
                                              uint32_t baudRate) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  AINFO << "BoostSerialDeviceManager::createDevice ";
  if (deviceMap_.find(port) == deviceMap_.end()) {
    AINFO << "Creating device at port: with baud rate: " << port;
    try {
      deviceMap_[port] = std::make_unique<BoostSerialDevice>(port, baudRate);
      auto status = deviceMap_[port]->open();
      if (!status.ok()) {
        AERROR << "Failed to open newly created device.";
        return status;
      }
      AINFO << "Device created and opened successfully.";
      return Status::OK();
    } catch (const boost::system::system_error& e) {
      const boost::system::error_code& ec = e.code();
      if (ec == boost::asio::error::already_open) {
        AWARN << "Device already open, attempting to close and reopen.";
        deviceMap_[port]->close(); // Try to close it first
        return deviceMap_[port]->open(); // Attempt to reopen
      } else if (ec == boost::asio::error::no_such_device ||
                 ec.value() == ENOENT) {
        std::string errorMessage = "Please make sure the device " + port +
                                   " is correct and accessible.";
        AERROR << "No such device: {}" << port;
        // return {STATUS::ERROR, ERROR::ERROR_OPENING_DEVICE, errorMessage};
        return Status(ErrorCode::CONTROL_ERROR,
                      std::string("ERROR_OPENING_DEVICE") + errorMessage);
      } else {
        AERROR << "Communication error while creating device: {}" << e.what();
        // return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR,
        // std::string(e.what())};
        return Status(
            ErrorCode::CONTROL_ERROR,
            std::string("COMMUNICATION_ERROR") + std::string(e.what()));
      }
    }
  }
  return Status::OK(); // Device already created
}

Status BoostSerialDeviceManager::writeToDevice(const std::string& port,
                                               std::string&& data) {
  std::lock_guard<std::mutex> lock(deviceMutex_);
  if (deviceMap_.find(port) == deviceMap_.end()) {
    AERROR << "Device not created, cannot write data.";
    // return {STATUS::ERROR, ERROR::SENSOR_FAILURE};
    return Status(ErrorCode::CONTROL_ERROR, std::string("SENSOR_FAILURE"));
  }

  AINFO << "Writing to device: " << toHexString(data);
  return deviceMap_[port]->write(data);
}
} // namespace solo::drivers::serial