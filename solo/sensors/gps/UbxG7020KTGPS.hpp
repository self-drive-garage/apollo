#pragma once

#include "platform/io/Status.hpp"
#include "platform/sensors/gps/idl/GPSData.hpp"

#include <libgpsmm.h>

#include <mutex>
#include <chrono>
#include <optional>
#include <tuple>

namespace platform::sensors {

class UbxG7020KTGPS
{
public:
    UbxG7020KTGPS();

    ~UbxG7020KTGPS();

    /// @brief Open the GPS device.
    ///
    /// Opens the GPS device and prepares it for use.
    /// @return Status indicating the result of the operation. Success or Error details.
    io::Status open();

    /// @brief Close the GPS device.
    ///
    /// Closes the GPS device and releases all resources.
    /// @return Status indicating the result of the operation. Success or Error details.
    io::Status close();

    /// @brief Read data or a response from the GPS device.
    ///
    /// Reads data or a response based on a request from the GPS device.
    /// @param timeout The timeout duration for the read operation.
    /// @return A tuple containing the status of the read operation and the optional data received.
    std::tuple<io::Status, std::optional<platform::sensors::GPSData>> read(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));


private:
    gpsmm gpsmmReceiver_; ///< The GPS receiver object.
    GPSData currentData_; ///< The current GPS data.

    const int MAX_ATTEMPTS = 5; ///< Maximum number of attempts to get valid data.
    const std::chrono::seconds WAIT_TIME = std::chrono::seconds(1); ///< Wait time for data in seconds.

};

} // namespace platform::sensors
