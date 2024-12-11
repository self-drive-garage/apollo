#pragma once

#include "platform/sensors/gps/GPSDeviceInterface.hpp"
#include "platform/io/Status.hpp"
#include "platform/sensors/gps/GPSData.hpp"

#include <gmock/gmock.h>

#include <optional>
#include <tuple>
#include <chrono>

namespace platform::sensors::mocks {

class MockGPSReceiver : public GPSDeviceInterface
{
public:
    MOCK_METHOD(io::Status, open, (), (override));
    MOCK_METHOD(io::Status, close, (), (override));
    MOCK_METHOD((std::tuple<io::Status, std::optional<GPSData>>), read, (std::chrono::milliseconds timeout), (override));
};

} // namespace platform::sensors::mocks
