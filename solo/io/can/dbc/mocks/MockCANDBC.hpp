#pragma once

#include "platform/io/can/dbc/CANDBC.hpp"

#include <gmock/gmock.h>

namespace platform::io::can::dbc::mocks {

class MockCANDBC : public CANDBC
{
public:
    MOCK_METHOD(std::optional<std::reference_wrapper<const CANDBCMessageSchema>>,
                getMessageByAddress,
                (uint32_t address));
    MOCK_METHOD(std::optional<std::reference_wrapper<const CANDBCMessageSchema>>,
                getMessageByName,
                (std::string messageName));
    MOCK_METHOD(std::optional<std::reference_wrapper<const std::vector<CANDBCSignalSchema>>>,
                getSignalSchemasByAddress,
                (uint32_t address));
};

} // namespace platform::io::can::dbc::mocks
