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

/// @file CANDBC.hpp
/// @brief This file contains the definition of the CANDBC class and related structs and enums.
/// @details Defines the CANDBC class which represents the schema for CAN DBC parsing and operations.

#pragma once

#include "platform/io/can/dbc/CANDBCMessageSchema.hpp"
#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace platform::io::can::dbc {

/// @class CANDBC
/// @brief Represents the CAN DBC schema and operations.
class CANDBC
{
public:
    /// @brief Data length code to number of bytes mapping.
    static constexpr uint8_t dataLengthCodeToNumBytes[] =
        {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};

    /// @brief Converts buffer size to data length code.
    /// @param len Buffer length in bytes.
    /// @return Corresponding data length code.
    static size_t bufferSizeToDataLengthCode(uint8_t len);

    /// @brief Creates an instance of CANDBC.
    /// @return A unique pointer to the CANDBC instance.
    static std::unique_ptr<CANDBC> CreateInstance() { return std::make_unique<CANDBC>(CANDBC()); }

    /// @brief Gets the message schema by address.
    /// @param address Address of the message.
    /// @return An optional reference to the message schema.
    std::optional<std::reference_wrapper<const CANDBCMessageSchema>> getMessageByAddress(uint32_t address);

    /// @brief Gets the message schema by name.
    /// @param messageName Name of the message.
    /// @return An optional reference to the message schema.
    std::optional<std::reference_wrapper<const CANDBCMessageSchema>> getMessageByName(std::string messageName);

    /// @brief Gets the signal schemas by address.
    /// @param address Address of the message.
    /// @return An optional reference to the vector of signal schemas.
    std::optional<std::reference_wrapper<const std::vector<CANDBCSignalSchema>>> getSignalSchemasByAddress(
        uint32_t address);

    /// @brief Represents the checksum state for Subaru CAN messages.
    struct ChecksumState
    {
        static const int checksumSize = 8;
        static const int counterSize = -1;
        static const int checksumStartBit = 0;
        static const int counterStartBit = -1;
        static const bool littleEndian = true;
    };

private:
    /// @brief Regular expression for message (BO_) parsing.
    std::regex boRegExp_{R"(^BO_ (\w+) (\w+) *: (\w+) (\w+))"};

    /// @brief Regular expression for signal (SG_) parsing.
    std::regex sgRegExp_{
        R"(^SG_ (\w+) : (\d+)\|(\d+)@(\d+)([\+|\-]) \(([0-9.+\-eE]+),([0-9.+\-eE]+)\) \[([0-9.+\-eE]+)\|([0-9.+\-eE]+)\] \"(.*)\" (.*))"};

    /// @brief Regular expression for multiplexed signal (SG_) parsing.
    std::regex sgmRegExp_{
        R"(^SG_ (\w+) (\w+) *: (\d+)\|(\d+)@(\d+)([\+|\-]) \(([0-9.+\-eE]+),([0-9.+\-eE]+)\) \[([0-9.+\-eE]+)\|([0-9.+\-eE]+)\] \"(.*)\" (.*))"};

    /// @brief Regular expression for signal value (VAL_) parsing.
    std::regex valRegExp_{R"(VAL_ (\w+) (\w+) (\s*[-+]?[0-9]+\s+\".+?\"[^;]*))"};

    /// @brief Regular expression for splitting signal values.
    std::regex valSplitRegExp_{R"([\"]+)"}; // split on "

    std::string name_ = std::string("Subaru Forester 2020");
    std::map<uint32_t, CANDBCMessageSchema> messagesAddressMap_;
    std::map<std::string, CANDBCMessageSchema> messagesNameMap_;

    /// @brief Trims whitespace from a string.
    /// @param s The string to trim.
    /// @param t The characters to trim.
    /// @return The trimmed string.
    std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

    /// @brief Checks if a string starts with a given prefix.
    /// @param str The string to check.
    /// @param prefix The prefix to check.
    /// @return True if the string starts with the prefix, otherwise false.
    static bool startswith(const std::string& str, const char* prefix);

    /// @brief Checks if a string starts with any of the given prefixes.
    /// @param str The string to check.
    /// @param prefix_list The list of prefixes to check.
    /// @return True if the string starts with any of the prefixes, otherwise false.
    static bool startswith(const std::string& str, std::initializer_list<const char*> prefix_list);

    /// @brief Checks if a string ends with a given suffix.
    /// @param str The string to check.
    /// @param suffix The suffix to check.
    /// @return True if the string ends with the suffix, otherwise false.
    bool endswith(const std::string& str, const char* suffix);

/// @brief Asserts a condition and throws an error if the condition is false.
/// @param condition The condition to check.
/// @param message The error message to throw if the condition is false.
#define DBC_ASSERT(condition, message)                                                                                 \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            std::stringstream is;                                                                                      \
            is << "[" << name_ << ":" << line_num << "] " << message;                                                  \
            throw std::runtime_error(is.str());                                                                        \
        }                                                                                                              \
    } while (false)

    /// @brief Constructs a CANDBC object.
    /// @param allow_duplicate_message_name Indicates whether duplicate message names are allowed.
    explicit CANDBC(bool allow_duplicate_message_name = false);
};

/// @brief Gets the size of the CAN header.
/// @return The size of the CAN header.
// constexpr size_t getSizeOfCANHeader();

/// @brief Gets the size of the data length array.
/// @return The size of the data length array.
// constexpr size_t getSizeOfDataLengthArray();

/// @brief Gets the size of a data length element.
/// @return The size of a data length element.
// constexpr size_t getSizeOfDataLengthElement();

} // namespace platform::io::can::dbc
