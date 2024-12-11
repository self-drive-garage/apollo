// SensorConcept.hpp

#pragma once

#include "platform/io/Status.hpp"
#include <chrono>
#include <optional>
#include <tuple>
#include <concepts>

namespace platform::sensors {

template<typename T, typename SensorData>
concept SensorConcept = requires(T sensor, std::chrono::milliseconds timeout) {
    // Open the sensor
    { sensor.open() } -> std::same_as<platform::io::Status>;

    // Close the sensor
    { sensor.close() } -> std::same_as<platform::io::Status>;

    // Initialize the sensor
    { sensor.initialize() } -> std::same_as<platform::io::Status>;

    // Read data from the sensor
    { sensor.read(timeout) } -> std::same_as<std::tuple<platform::io::Status, std::optional<SensorData>>>;

    // Check if the sensor is opened
    { sensor.isOpened() } -> std::convertible_to<bool>;

    // Check if the sensor is initialized
    { sensor.isInitialized() } -> std::convertible_to<bool>;

    // Calibrate the sensor
    { sensor.calibrate() } -> std::same_as<platform::io::Status>;

    // Check if the sensor is connected
    { sensor.isConnected() } -> std::convertible_to<bool>;
};

} // namespace platform::sensors
