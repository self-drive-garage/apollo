// // GPSReceiverConcept.hpp

// #pragma once

// #include "platform/io/Status.hpp"
// #include "platform/sensors/gps/GPSData.hpp"

// #include <chrono>
// #include <tuple>
// #include <optional>

// namespace platform::sensors {

// template<typename T>
// concept GPSReceiverConcept = requires(T t, std::chrono::milliseconds timeout) {
//     { t.open() } -> std::same_as<io::Status>;
//     { t.close() } -> std::same_as<io::Status>;
//     { t.read(timeout) } -> std::same_as<std::tuple<io::Status, std::optional<GPSData>>>;
// };

// } // namespace platform::sensors
