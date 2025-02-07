#pragma once

#include <chrono>

namespace platform {

// 10 Hz update rate
constexpr std::chrono::milliseconds TEMPORARY_GLOBALS_UPDATE_INTERVAL = std::chrono::milliseconds(100);


} // namespace platform