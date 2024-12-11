// MIT License
//
// Copyright (c) 2024 Self Drive Garage
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// @file SocketCANMessage.hpp
/// @brief This file contains the implementation of the SocketCANMessage class.
/// @date 2024
/// @details Defines the SocketCANMessage class which is a convenience wrapper for the Linux Socket CAN struct
/// `can_frame`.
///
#include "platform/io/can/SocketCANMessage.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstring>

namespace platform::io::can {

SocketCANMessage::SocketCANMessage(can_frame frame)
    : canId_(frame.can_id)
    , frameData_(frame.data, frame.data + frame.can_dlc)
    , rawFrame_(frame)
{
    ADEBUG << "SocketCANMessage created with CAN ID: {}", canId_);
}

SocketCANMessage::SocketCANMessage(uint32_t canId, std::vector<uint8_t> frameData)
    : canId_(canId)
    , frameData_(std::move(frameData))
{
    memset(&rawFrame_, 0, sizeof(rawFrame_));
    rawFrame_.can_id = canId_;
    rawFrame_.can_dlc = frameData_.size();
    std::copy(frameData_.begin(), frameData_.end(), rawFrame_.data);
    ADEBUG << "SocketCANMessage created with CAN ID: {} and frame data size: {}", canId_, frameData_.size());
}

uint32_t SocketCANMessage::getAddress() const
{
    return canId_ & CAN_EFF_MASK;
}

bool SocketCANMessage::isValid() const
{
    return !frameData_.empty();
}

uint32_t SocketCANMessage::getCanId() const
{
    return canId_;
}

const std::vector<uint8_t>& SocketCANMessage::getFrameData() const
{
    return frameData_;
}

can_frame SocketCANMessage::getRawFrame() const
{
    return rawFrame_;
}

SocketCANMessage SocketCANMessage::fromCANDBCMessage(const dbc::CANDBCMessage& canDBCMessage,
                                                     std::vector<uint8_t> frameData)
{
    ADEBUG << "Creating SocketCANMessage from CANDBCMessage with address: {}", canDBCMessage.address());
    return {canDBCMessage.address(), std::move(frameData)};
}

SocketCANMessage SocketCANMessage::fromRawData(const uint32_t address, const std::vector<uint8_t>& rawData)
{
    ADEBUG << "Creating SocketCANMessage from raw data with address: {}", address);
    return {address, std::move(rawData)};
}
} // namespace platform::io::can
