#pragma once

#include "platform/io/Status.hpp"
#include "platform/sensors/gps/GPSData.hpp"

#include <chrono>
#include <optional>
#include <tuple>

namespace platform::sensors {

class GPSDeviceInterface
{
public:

    /// @brief Open the GPS device.
    ///
    /// Opens the GPS device and prepares it for use.
    /// @return Status indicating the result of the operation. Success or Error details.
    virtual io::Status open() = 0;

    /// @brief Close the GPS device.
    ///
    /// Closes the GPS device and releases all resources.
    /// @return Status indicating the result of the operation. Success or Error details.
    virtual io::Status close() = 0;

    /// @brief Read data or a response from the GPS device.
    ///
    /// Reads data or a response based on a request from the GPS device.
    /// @param timeout The timeout duration for the read operation.
    /// @return A tuple containing the status of the read operation and the optional data received.
    virtual std::tuple<io::Status, std::optional<GPSData>> read(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) = 0;
  
};

} // namespace platform::sensors
