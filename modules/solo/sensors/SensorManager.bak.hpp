// #pragma once


// #include <chrono>
// #include <map>
// #include <string>
// #include <optional>  

// namespace platform::sensors {

// template<typename SensorData>
// class Sensor;

// /// @class SensorManager
// /// @brief A class that manages sensors and provides a unified interface for reading data from them
// template<typename SensorData>
// class SensorManager
// {
// public:

//     /// @brief Adds a sensor to the manager
//     /// @param id The unique identifier for the sensor
//     /// @param sensor The sensor object to be added
//     /// @note The sensor is moved into the manager, transferring ownership
//     virtual void addSensor(const std::string& sensorId, Sensor<SensorData>&& sensor) = 0;

//     /// @brief Removes a sensor from the manager
//     /// @param id The unique identifier for the sensor
//     virtual void removeSensor(const std::string& sensorId) = 0;

//     /// @brief Reads data from a sensor
//     /// @param id The unique identifier for the sensor
//     /// @return The data read from the sensor
//     virtual std::optional<SensorData> readSensorData(const std::string& sensorId, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) = 0;

// private:
//     std::map<std::string, Sensor<SensorData>> sensors_;
// };

// } // namespace platform::sensors
