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

/// @file CAN.hpp
/// @brief This file contains the definition of the CANBus, CANHeader, and CANFrame.
/// @details This file contains the definition of the CANBus, CANHeader, and CANFrame.

#pragma once

#include <cstdint>
#include <vector>

namespace platform::io::can {

/// @enum CANBus
/// @brief Enumerates the CAN buses.
enum class CANBus
{
    MAIN_BUS = 0,  ///< Main bus
    ALT_BUS = 1,   ///< Alternate bus
    CAMERA_BUS = 2 ///< Camera bus
};

/// @struct CANHeader
/// @brief Represents the CAN header with packed attributes.
struct __attribute__((packed)) CANHeader
{
    uint8_t reserved : 1;       ///< Reserved bit
    uint8_t bus : 3;            ///< Bus
    uint8_t dataLengthCode : 4; ///< Data length code
    uint8_t rejected : 1;       ///< Rejected bit
    uint8_t returned : 1;       ///< Returned bit
    uint8_t extended : 1;       ///< Extended bit
    uint32_t addr : 29;         ///< Address
    uint8_t checksum : 8;       ///< Checksum
};

/// @struct CANFrame
/// @brief Represents a CAN frame.
struct CANFrame
{
    long address;                 ///< Address
    std::vector<uint8_t> data;    ///< Data
    long busTime;                 ///< Bus time
    long src;                     ///< Source
    CANBus canBus;                ///< CAN bus
    uint8_t counter = 0;          ///< Counter
    uint8_t numCounterErrors = 0; ///< Number of counter errors
};

} // namespace platform::io::can
