#pragma once

#include "modules/common/status/status.h"
#include "solo/modules/drivers/serial/BoostSerialDevice.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>

namespace solo::drivers::serial {

using namespace apollo::common;

/// @class BoostSerialDeviceManager
/// @brief Thread safe management of the operations for Serial Device.
///
/// This class handles the lifecycle and operations of a serial device,
/// ensuring proper initialization, opening, and closing of the device.
/// It provides methods for reading from and writing to the device,
/// abstracting the complexity of direct device manipulation. Designed to
/// manage a single device instance across multiple consumers while being thread safe.
class BoostSerialDeviceManager
{
public:
    /// @brief Default constructor for BoostSerialDeviceManager.
    BoostSerialDeviceManager() = default;

    /// @brief Move constructor for BoostSerialDeviceManager.
    BoostSerialDeviceManager(BoostSerialDeviceManager&& other) noexcept
    {
        std::lock_guard<std::mutex> lock(other.deviceMutex_);
        deviceMap_ = std::move(other.deviceMap_);
    }

    /// @brief Move assignment operator for BoostSerialDeviceManager.
    BoostSerialDeviceManager& operator=(BoostSerialDeviceManager&& other) noexcept
    {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(other.deviceMutex_);
            std::lock_guard<std::mutex> thisLock(deviceMutex_);
            deviceMap_ = std::move(other.deviceMap_);
        }
        return *this;
    }

    /// @brief Destructor that ensures the device is closed properly.
    ~BoostSerialDeviceManager()
    {
        std::lock_guard<std::mutex> lock(deviceMutex_);
        for (const auto& [deviceName, device] : deviceMap_) {
            device->close();
        }
    }

    /// @brief Creates the serial device if it has not already been created.
    /// @param port The serial port name.
    /// @param baudRate The communication baud rate.
    /// @note This method is thread-safe and ensures that only one device is created.
    Status createDevice(const std::string& port, uint32_t baudRate);

    /// @brief Initializes the serial device.
    /// @return Status indicating the success or failure of the initialization.
    Status initializeDevice(const std::string& port);

    /// @brief Opens the serial device if it is not already open.
    /// @return Status indicating the success or failure of the operation.
    Status openDevice(const std::string& port);

    /// @brief Closes the serial device if it is open.
    /// @return Status indicating the success or failure of the operation.
    Status closeDevice(const std::string& port);

    /// @brief Reads data from the serial device based on a given request.
    /// @param request The data or command to send to the device before reading the response.
    /// @return A tuple containing the status of the operation and the optional read data.
    std::tuple<Status, std::optional<std::string>> readFromDevice(const std::string& port, const std::string& request);

    /// @brief Writes data to the serial device.
    /// @param data The data or command to write to the device.
    /// @return Status indicating the success or failure of the write operation.
    Status writeToDevice(const std::string& port, std::string&& data);

private:
    std::mutex deviceMutex_; ///< Mutex to synchronize access to the device.
    std::unordered_map<std::string, std::unique_ptr<BoostSerialDevice>> deviceMap_;

    std::string toHexString(const std::string& data)
    {
        std::stringstream ss;
        for (unsigned char c : data) {
            ss << static_cast<int>(c) << " "; // Print the integer value of each byte
        }
        return ss.str();
    }
};

} // namespace solo::drivers::serial
