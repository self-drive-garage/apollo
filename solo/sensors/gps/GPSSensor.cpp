#include "platform/sensors/gps/GPSSensor.hpp"
#include "platform/sensors/gps/UbxG7020KTGPS.hpp"
#include "platform/io/Status.hpp"

#include <spdlog/spdlog.h>
namespace platform::sensors {


GPSSensor::GPSSensor(UbxG7020KTGPS gpsReceiver)
    : gpsReceiver_{std::move(gpsReceiver)} {
}


GPSSensor::~GPSSensor() {
    if (isOpened()) {
        close();
    }
}


io::Status GPSSensor::open() {
    // Check if the device is already open
    if (isOpened()) {
        return {io::STATUS::ERROR, io::ERROR::ALREADY_OPEN};
    }

    auto status = gpsReceiver_.open();

    isOpened_ = status.ok();

    if (isOpened_) {
        AINFO << "GPS device opened successfully.");
        return status;
    }

    AERROR << "Failed to open GPS device.");
    return status;
}


io::Status GPSSensor::close() {
    if (!isOpened()) {
        return {io::STATUS::ERROR, io::ERROR::ALREADY_CLOSED};
    }

    auto status = gpsReceiver_.close();

    if (status.ok()) {
        isOpened_ = false;
        AINFO << "GPS device closed successfully.");
    }
    return status;
}


bool GPSSensor::isOpened() const {
    return isOpened_;
}

    
std::tuple<io::Status, std::optional<GPSData>> GPSSensor::read(std::chrono::milliseconds timeout) {
   if (!isOpened()) {
        return std::make_tuple(io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::ERROR_OPENING_DEVICE}, std::nullopt);
    }

    // Wait for data to be available
    auto [status, data] = gpsReceiver_.read(timeout);

    if (status.ok()) {
        return std::make_tuple(status, data);
    }

    return std::make_tuple(status, std::nullopt);

}


io::Status GPSSensor::calibrate() {
    return io::Status{platform::io::STATUS::SUCCESS};
}


bool GPSSensor::isConnected() const {
    return isConnected_;
}           


bool GPSSensor::isInitialized() const {
    return isInitialized_;
}


io::Status GPSSensor::initialize() {
    return io::Status{platform::io::STATUS::SUCCESS};
}

} // namespace platform::sensors
