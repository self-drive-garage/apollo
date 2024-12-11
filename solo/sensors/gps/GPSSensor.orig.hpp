#pragma once

#include "platform/sensors/gps/GPSData.hpp"
#include "platform/io/Status.hpp"
#include "platform/sensors/gps/GPSReceiverConcept.hpp"


#include <memory>
#include <optional>
#include <chrono>

namespace platform::sensors {

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
class GPSSensor {
public:
    /// @brief Construct a new GPSSensor object.
    ///
    /// @param gpsReceiver The GPS receiver to use.
    GPSSensor(std::unique_ptr<GPSReceiver> gpsReceiver);

    // /// @brief Construct a new GPSSensor object.
    // ///
    // /// @param gpsReceiver The GPS receiver to use.
    // GPSSensor(GPSReceiver&& gpsReceiver);

    // Define special member functions
    ~GPSSensor();

    GPSSensor(const GPSSensor& other) = delete;

    GPSSensor& operator=(const GPSSensor& other) = delete;
    
    GPSSensor(GPSSensor&& other) noexcept = default;
    
    GPSSensor& operator=(GPSSensor&& other) noexcept = default;

    /// @brief Open the GPS device.
    ///
    /// Opens the GPS device and prepares it for use.
    /// @return Status indicating the result of the operation. Success or Error details.
    io::Status open() ;

    /// @brief Close the GPS device.
    ///
    /// Closes the GPS device and releases all resources.
    /// @return Status indicating the result of the operation. Success or Error details.
    io::Status close() ;

    /// @brief Reopen the GPS device.
    ///
    /// Closes the GPS device if open, and reopens it.
    /// @return Status indicating the result of the operation. Success or Error details.
    io::Status reopen() ;

    /// @brief Initialize the GPS device after opening.
    ///
    /// Performs necessary initialization steps after opening the GPS device.
    /// @return Status indicating the success or failure of the initialization.
    io::Status initialize() ;

    /// @brief Read data or a response from the GPS device.
    ///
    /// Reads data or a response based on a request from the GPS device.
    /// @param timeout The timeout duration for the read operation.
    /// @return A tuple containing the status of the read operation and the optional data received.
    std::tuple<io::Status, std::optional<GPSData>> read(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) ;

    /// @brief Check if the GPS device is currently open.
    ///
    /// @return True if the GPS device is open, otherwise false.
    bool isOpened() const ;

    /// @brief Check if the GPS device has been initialized after opening.
    ///
    /// @return True if the GPS device has been initialized, otherwise false.
    bool isInitialized() const ;

    /// @brief Calibrate the GPS device.
    ///
    /// @return Status indicating the success or failure of the calibration.
    io::Status calibrate() ;

    /// @brief Check if the GPS device is connected.
    ///
    /// @return True if the GPS device is connected, otherwise false.
    bool isConnected() const ;

private:
    bool isOpened_{false}; ///< Flag to track if the device is opened.
    bool isInitialized_{false}; ///< Flag to track if the device has been initialized.
    bool isConnected_{false}; ///< Flag to track if the device is connected.

    std::unique_ptr<GPSReceiver> gpsReceiver_; ///< Pointer to the GPS receiver object.
    GPSData currentData_{}; ///< Current GPS data.
};

} // namespace platform::sensors



namespace platform::sensors {


template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
GPSSensor<GPSReceiver>::GPSSensor(std::unique_ptr<GPSReceiver> gpsReceiver)
    : gpsReceiver_{std::move(gpsReceiver)} {
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
GPSSensor<GPSReceiver>::~GPSSensor() {
    if (isOpened()) {
        close();
    }
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
io::Status GPSSensor<GPSReceiver>::open() {
    // Check if the device is already open
    if (isOpened()) {
        return {io::STATUS::ERROR, io::ERROR::ALREADY_OPEN};
    }

    auto status = gpsReceiver_->open();

    isOpened_ = status.ok();

    return status;
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
io::Status GPSSensor<GPSReceiver>::close() {
    if (!isOpened()) {
        return {io::STATUS::ERROR, io::ERROR::ALREADY_CLOSED};
    }

    auto status = gpsReceiver_->close();

    if (status.ok()) {
        isOpened_ = false;
    }
    return status;
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
bool GPSSensor<GPSReceiver>::isOpened() const {
    return isOpened_;
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>    
std::tuple<io::Status, std::optional<GPSData>> GPSSensor<GPSReceiver>::read(std::chrono::milliseconds timeout) {
   if (!isOpened()) {
        return std::make_tuple(io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::ERROR_OPENING_DEVICE}, std::nullopt);
    }

    // Wait for data to be available
    auto [status, data] = gpsReceiver_->read(timeout);

    if (status.ok()) {
        return std::make_tuple(status, data);
    }

    return std::make_tuple(status, std::nullopt);

}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
io::Status GPSSensor<GPSReceiver>::calibrate() {
    return io::Status{platform::io::STATUS::SUCCESS};
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
bool GPSSensor<GPSReceiver>::isConnected() const {
    return isConnected_;
}           

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
bool GPSSensor<GPSReceiver>::isInitialized() const {
    return isInitialized_;
}

template <typename GPSReceiver>
requires GPSReceiverConcept<GPSReceiver>
io::Status GPSSensor<GPSReceiver>::initialize() {
    return io::Status{platform::io::STATUS::SUCCESS};
}

} // namespace platform::sensors
