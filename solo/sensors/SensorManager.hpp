#pragma once

#include "platform/io/Status.hpp"
#include "platform/sensors/SensorConcept.hpp"

#include <exception>
#include <mutex>
#include <optional>
#include <tuple>

namespace platform::sensors {

/// @class SensorManager
/// @brief Interface class for sensor operations.
template<typename Sensor, typename SensorData>
requires SensorConcept<Sensor, SensorData>
class SensorManager
{

public:
    virtual ~SensorManager() = default;

    void addSensor(std::shared_ptr<Sensor> sensor);

protected:
    std::vector<std::unique_ptr<Sensor>> sensors_; ///< List of sensors managed
    mutable std::mutex sensorMutex_; ///< Mutex for thread-safe operations


};

#include "platform/sensors/SensorManager.inl"

} // namespace platform::sensors


namespace platform::sensors {


template<typename Sensor, typename SensorData>
requires SensorConcept<Sensor, SensorData>
void SensorManager<Sensor, SensorData>::addSensor(std::shared_ptr<Sensor> sensor) {
    std::lock_guard<std::mutex> lock(sensorMutex_);
    sensors_.push_back(std::move(sensor));
}


// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// platform::io::Status SensorManager<Sensor, SensorData>::open() {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
    
//     if (sensor_.open()) {
//         isOpened_ = true;
//         return {platform::io::STATUS::SUCCESS};
//     }
//     return {platform::io::STATUS::ERROR, platform::io::ERROR::SENSOR_NOT_OPENED};
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// platform::io::Status SensorManager<Sensor, SensorData>::close() {
//     std::lock_guard<std::mutex> lock(sensorMutex_);
    
//     if (sensor_.close()) {
//         isOpened_ = false;
//         return {platform::io::STATUS::SUCCESS};
//     }
//     return {platform::io::STATUS::ERROR, platform::io::ERROR::ALREADY_CLOSED};
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// platform::io::Status SensorManager<Sensor, SensorData>::initialize() {
//     std::lock_guard<std::mutex> lock(sensorMutex_);
    
//     if (sensor_.initialize()) {
//         isInitialized_ = true;
//         return {platform::io::STATUS::SUCCESS};
//     }
//     return {platform::io::STATUS::ERROR, platform::io::ERROR::SENSOR_NOT_INITIALIZED};
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// bool SensorManager<Sensor, SensorData>::isConnected() const {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
//     return sensor_.isConnected();
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// bool SensorManager<Sensor, SensorData>::isInitialized() const {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
//     return isInitialized_;
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// platform::io::Status SensorManager<Sensor, SensorData>::calibrate() {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
    
//     if (sensor_.calibrate()) {
//         isCalibrated_ = true;
//         return {platform::io::STATUS::SUCCESS};
//     }
//     return {platform::io::STATUS::ERROR, platform::io::ERROR::SENSOR_FAILURE};
// }

// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// bool SensorManager<Sensor, SensorData>::isOpened() const {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
//     return isOpened_;
// }

// // TODO: we should return data for three or 4 
// // times and then return null opt
// // might stabelize the system
// // or give change for the sensor to recover
// template<typename Sensor, typename SensorData>
// requires SensorConcept<Sensor, SensorData>
// std::tuple<platform::io::Status, std::optional<SensorData>> SensorManager<Sensor, SensorData>::read(std::chrono::milliseconds timeout) {
//     // std::lock_guard<std::mutex> lock(sensorMutex_);
   
//     auto [status, data] = sensor_.read(timeout);
//     if (status == platform::io::STATUS::SUCCESS) {
//         currentData_ = data;
//         return {status, data};
//     }
//     return {status, std::nullopt};
// }

} // namespace platform::sensors
