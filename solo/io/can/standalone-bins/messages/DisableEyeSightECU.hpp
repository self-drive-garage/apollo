#pragma once

#include "platform/io/can/CANClient.hpp"
#include "platform/io/can/SocketCANDevice.hpp"
#include "spdlog/spdlog.h"



bool disableEyeSightECU()
{
    // Define the disable command
    std::vector<uint8_t> disableCommand = {0x28, 0x03, 0x01};

    // Create a CANDBCMessage for the disable command
    auto disableMessage = canClient->createCANDBCMessage("GLOBAL_ES_ADDR", {{"DATA", disableCommand}});

    if (!disableMessage) {
        AERROR << "Failed to create disable command message.");
        return false;
    }

    // Send the disable command
    bool success = canClient->sendMessage<SocketCANMessage>(*disableMessage);
    if (success) {
        AINFO << "Successfully sent disable command to EyeSight ECU.");
    } else {
        AERROR << "Failed to send disable command to EyeSight ECU.");
    }

    return success;
}
