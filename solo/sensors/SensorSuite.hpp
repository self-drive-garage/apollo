#pragma once

#include "platform/sensors/SensorManager.hpp"
#include "platform/io/Status.hpp"
#include "platform/sensors/gps/GPSData.hpp"
#include "platform/sensors/gps/GPSSensor.hpp"
// #include "platform/sensors/camera/CameraSensor.hpp"

#include <memory>
#include <vector>
#include <optional>
#include <tuple>
#include <chrono>

namespace platform::sensors {

class SensorSuite {
public:
    SensorSuite();

    // Initialize all sensor managers
    platform::io::Status initializeSensors();

    // Camera methods
    // std::tuple<platform::io::Status, std::vector<CameraData>> grabSynchronizedFrames();
    // std::tuple<platform::io::Status, CameraData> grabFrame(const std::string& cameraId);

    // GPS methods
    std::tuple<platform::io::Status, GPSData> getGPSData();

    // Get synchronized data from all sensors
    // std::tuple<platform::io::Status, std::tuple<std::vector<CameraData>, GPSData>> getSynchronizedData();

private:
    // Sensor Managers
    // std::shared_ptr<SensorManager<CameraSensor, CameraData>> cameraManager_;
    std::shared_ptr<SensorManager<GPSSensor<GPSReceiver>, GPSData>> gpsManager_;

    // Helper methods
    platform::io::Status initializeCameraManager();
    platform::io::Status initializeGPSManager();
};

} // namespace platform::sensors
