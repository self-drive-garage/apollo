#pragma once

#include "platform/io/gps/GPSReader.hpp"
#include "platform/io/Status.hpp"

#include <gmock/gmock.h>

#include <chrono>
#include <optional>
#include <tuple>

namespace platform::io::mocks {

class MockGpsOps : public platform::io::gps::GPSInterface
{
public:
    MOCK_METHOD(Status, open, (), (override));
    MOCK_METHOD(Status, close, (), (override));
    MOCK_METHOD(bool, isOpened, (), (const, override));
    MOCK_METHOD((std::tuple<Status, std::optional<platform::io::gps::GPSData>>), read, (std::chrono::milliseconds timeout), (override));
    MOCK_METHOD(Status, calibrate, (), (override));
};

} // namespace platform::io::mocks
