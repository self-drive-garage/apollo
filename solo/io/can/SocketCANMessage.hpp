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
/// @brief This file contains the definition of the SocketCANMessage class.
/// @date 2024
/// @details Defines the SocketCANMessage class which is a convenience wrapper for the Linux Socket CAN struct
/// `can_frame`.

#pragma once

#include "platform/io/can/dbc/CANDBCMessage.hpp"

#include <linux/can.h>

#include <system_error>
#include <vector>

namespace platform::io::can {

/// @class SocketCANMessage
/// @brief A convenience wrapper for the Linux Socket CAN struct `can_frame`.
class SocketCANMessage
{
public:
    /// @brief Constructs a SocketCANMessage from a `can_frame`.
    /// @param frame The `can_frame` to construct the message from.
    explicit SocketCANMessage(can_frame frame);

    /// @brief Constructs a SocketCANMessage from a CAN ID and frame data.
    /// @param canId The CAN ID.
    /// @param frameData The frame data.
    SocketCANMessage(uint32_t canId, std::vector<uint8_t> frameData);

    /// @brief Gets the CAN address.
    /// @return The CAN address.
    uint32_t getAddress() const;

    /// @brief Checks if the message is valid.
    /// @return True if the message is valid, otherwise false.
    bool isValid() const;

    /// @brief Gets the CAN ID.
    /// @return The CAN ID.
    uint32_t getCanId() const;

    /// @brief Gets the frame data.
    /// @return A reference to the frame data vector.
    const std::vector<uint8_t>& getFrameData() const;

    /// @brief Gets the raw `can_frame`.
    /// @return The raw `can_frame`.
    can_frame getRawFrame() const;

    /// @brief Constructs a SocketCANMessage from a CAN DBC message and frame data.
    /// @param canDBCMessage The CAN DBC message.
    /// @param frameData The frame data.
    /// @return The constructed SocketCANMessage.
    static SocketCANMessage fromCANDBCMessage(const dbc::CANDBCMessage& canDBCMessage, std::vector<uint8_t> frameData);

    static SocketCANMessage fromRawData(uint32_t address, const std::vector<uint8_t>& rawData);

private:
    uint32_t canId_;                 ///< The CAN ID.
    std::vector<uint8_t> frameData_; ///< The frame data.
    struct can_frame rawFrame_;      ///< The raw `can_frame`.
};

} // namespace platform::io::can
