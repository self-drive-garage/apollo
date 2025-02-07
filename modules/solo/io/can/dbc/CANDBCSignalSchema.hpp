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

/// @file CANDBCSignalSchema.hpp
/// @brief This file contains the definition of the CANDBCSignalSchema class.
/// @details Defines the CANDBCSignalSchema class which represents the schema of a CAN DBC signal.

#pragma once

#include "platform/io/can/dbc/CANDBCMessage.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace platform::io::can::dbc {

/// @enum SignalType
/// @brief Enumerates the types of signals in CAN DBC.
enum class SignalType
{
    DEFAULT,  ///< Default signal type
    COUNTER,  ///< Counter signal type
    CHECKSUM, ///< Checksum signal type
};

/// @class CANDBCSignalSchema
/// @brief Represents the schema of a CAN DBC signal.
class CANDBCSignalSchema
{
public:
    /// @struct ValueDescription
    /// @brief Describes a value in the CAN DBC signal.
    struct ValueDescription
    {
        std::string name;    ///< Name of the value
        uint32_t address;    ///< Address of the value
        std::string def_val; ///< Default value as a string
    };

    CANDBCSignalSchema() = default;

    /// @brief Constructs a CANDBCSignalSchema object.
    /// @param name Name of the signal.
    /// @param message_name Name of the message.
    /// @param start_bit Start bit of the signal.
    /// @param most_significant_bit Most significant bit of the signal.
    /// @param least_significant_bit Least significant bit of the signal.
    /// @param size Size of the signal in bits.
    /// @param is_signed Indicates if the signal is signed.
    /// @param factor Factor to scale the signal value.
    /// @param offset Offset to be added to the signal value.
    /// @param is_little_endian Indicates if the signal is in little-endian format.
    /// @param type Type of the signal.
    /// @param value_description Description of the value.
    CANDBCSignalSchema(const std::string& name,
                       const std::string& message_name,
                       int start_bit,
                       int most_significant_bit,
                       int least_significant_bit,
                       int size,
                       bool is_signed,
                       double factor,
                       double offset,
                       bool is_little_endian,
                       SignalType type,
                       const ValueDescription& value_description);

    /// @brief Gets the name of the signal.
    /// @return The name of the signal.
    const std::string& getName() const;

    /// @brief Gets the name of the message.
    /// @return The name of the message.
    const std::string& getMessageName() const;

    /// @brief Gets the start bit of the signal.
    /// @return The start bit of the signal.
    int getStartBit() const;

    /// @brief Gets the most significant bit of the signal.
    /// @return The most significant bit of the signal.
    int getMostSignificantBit() const;

    /// @brief Gets the least significant bit of the signal.
    /// @return The least significant bit of the signal.
    int getLeastSignificantBit() const;

    /// @brief Gets the size of the signal in bits.
    /// @return The size of the signal.
    int getSize() const;

    /// @brief Checks if the signal is signed.
    /// @return True if the signal is signed, otherwise false.
    bool isSigned() const;

    /// @brief Gets the factor to scale the signal value.
    /// @return The factor to scale the signal value.
    double getFactor() const;

    /// @brief Gets the offset to be added to the signal value.
    /// @return The offset to be added to the signal value.
    double getOffset() const;

    /// @brief Checks if the signal is in little-endian format.
    /// @return True if the signal is in little-endian format, otherwise false.
    bool isLittleEndian() const;

    /// @brief Gets the type of the signal.
    /// @return The type of the signal.
    const SignalType& getSignalType() const;

    /// @brief Calculates the Subaru checksum.
    /// @param address The address of the data.
    /// @param d The data vector.
    /// @return The calculated checksum.
    static uint8_t calcSubaruChecksum(uint32_t address, const std::vector<uint8_t>& d);

    /// @brief Parses the value from the message data.
    /// @param messageData The message data vector.
    /// @return The parsed value.
    int64_t parseValue(const std::vector<uint8_t>& messageData) const;

    /// @brief Sets the name of the signal.
    /// @param name The name of the signal.
    void setName(std::string name);

    /// @brief Sets the name of the message.
    /// @param messageName The name of the message.
    void setMessageName(const std::string& messageName);

    /// @brief Sets the start bit of the signal.
    /// @param startBit The start bit of the signal.
    void setStartBit(int startBit);

    /// @brief Sets the most significant bit of the signal.
    /// @param mostSignificantBit The most significant bit of the signal.
    void setMostSignificantBit(int mostSignificantBit);

    /// @brief Sets the least significant bit of the signal.
    /// @param leastSignificantBit The least significant bit of the signal.
    void setLeastSignificantBit(int leastSignificantBit);

    /// @brief Sets the size of the signal.
    /// @param size The size of the signal.
    void setSize(int size);

    /// @brief Sets if the signal is signed.
    /// @param isSigned Indicates if the signal is signed.
    void setIsSigned(bool isSigned);

    /// @brief Sets the factor to scale the signal value.
    /// @param factor The factor to scale the signal value.
    void setFactor(double factor);

    /// @brief Sets the offset to be added to the signal value.
    /// @param offset The offset to be added to the signal value.
    void setOffset(double offset);

    /// @brief Sets if the signal is in little-endian format.
    /// @param isLittleEndian Indicates if the signal is in little-endian format.
    void setIsLittleEndian(bool isLittleEndian);

    /// @brief Gets the type of the signal.
    /// @return The type of the signal.
    SignalType getType() const;

    /// @brief Sets the type of the signal.
    /// @param type The type of the signal.
    void setType(SignalType type);

    /// @brief Gets the value description of the signal.
    /// @return The value description of the signal.
    const ValueDescription& getValueDescription() const;

    /// @brief Sets the value description of the signal.
    /// @param valueDescription The value description of the signal.
    void setValueDescription(const ValueDescription& valueDescription);

    /// @brief Packs the value into the write buffer.
    /// @param writeBuffer The write buffer vector.
    /// @param rawValue The value to be packed.
    void packValue(std::vector<uint8_t>& writeBuffer, int64_t rawValue) const;

    /// @brief Get now as nanoseconds
    /// TO-DO: move this out of here
    uint64_t get_now_as_nanoseconds() const
    {

        // Get the current time point
        auto now = std::chrono::high_resolution_clock::now();

        // Convert the time point to nanoseconds
        auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);

        // Get the duration since epoch
        auto duration_since_epoch = now_ns.time_since_epoch();

        // Convert to nanoseconds
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch).count();

        return nanoseconds;
    }

    /// @brief Construct a CANDBCSignal using the internal schema
    /// @param value the value of the signal to be sent
    CANDBCSignal createCANDBCSignal(double value) const;

private:
    std::string name_;                  ///< Name of the signal.
    std::string messageName_;           ///< Name of the message.
    int startBit_;                      ///< Start bit of the signal.
    int mostSignificantBit_;            ///< Most significant bit of the signal.
    int leastSignificantBit_;           ///< Least significant bit of the signal.
    int size_;                          ///< Size of the signal in bits.
    bool isSigned_;                     ///< Indicates if the signal is signed.
    double factor_;                     ///< Factor to scale the signal value.
    double offset_;                     ///< Offset to be added to the signal value.
    bool isLittleEndian_;               ///< Indicates if the signal is in little-endian format.
    SignalType type_;                   ///< Type of the signal.
    ValueDescription valueDescription_; ///< Description of the value.
};

} // namespace platform::io::can::dbc
