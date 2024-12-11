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

/// @file CANDBCMessageSchema.hpp
/// @brief This file contains the definition of the CANDBCMessageSchema class.
/// @details Defines the CANDBCMessageSchema class which represents the schema of a CAN DBC message.

#pragma once

#include "platform/io/can/dbc/CANDBCMessage.hpp"
#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace platform::io::can::dbc {

/// @class CANDBCMessageSchema
/// @brief Represents the schema of a CAN DBC message.
class CANDBCMessageSchema
{
public:
    CANDBCMessageSchema() = default;
    /// @brief Constructs a CANDBCMessageSchema object.
    /// @param name Name of the message.
    /// @param address Address of the message.
    /// @param size Size of the message in bytes.
    /// @param signals Vector of signal schemas associated with the message.
    /// @param signal_name_to_signal_map Map from signal names to signal schemas.
    CANDBCMessageSchema(std::string name,
                        uint32_t address,
                        uint32_t size,
                        const std::vector<CANDBCSignalSchema>& signals,
                        const std::unordered_map<std::string, const CANDBCSignalSchema>& signal_name_to_signal_map);

    /// @brief Gets the name of the message.
    /// @return The name of the message.
    const std::string& getName() const;

    /// @brief Sets the name of the message.
    /// @param name The new name of the message.
    void setName(std::string name);

    /// @brief Gets the address of the message.
    /// @return The address of the message.
    uint32_t getAddress() const;

    /// @brief Sets the address of the message.
    /// @param address The new address of the message.
    void setAddress(uint32_t address);

    /// @brief Gets the size of the message in bytes.
    /// @return The size of the message.
    uint32_t getSize() const;

    /// @brief Sets the size of the message.
    /// @param size The new size of the message.
    void setSize(uint32_t size);

    /// @brief Gets the signal schema by name.
    /// @param signalName The name of the signal.
    /// @return An optional reference to the signal schema.
    std::optional<std::reference_wrapper<const CANDBCSignalSchema>> getSignalSchemaByName(
        const std::string& signalName) const;

    /// @brief Moves a signal schema to the map.
    /// @param signalSchemaName The name of the signal schema.
    /// @param signalSchema The signal schema to be moved.
    void moveSignalSchemaToMap(std::string signalSchemaName, CANDBCSignalSchema signalSchema);

    /// @brief Gets the map from signal names to signal schemas.
    /// @return The map from signal names to signal schemas.
    const std::unordered_map<std::string, const CANDBCSignalSchema>& getSignalNameToSignalMap() const;

    /// @brief Sets the map from signal names to signal schemas.
    /// @param signalNameToSignalMap The new map from signal names to signal schemas.
    void setSignalNameToSignalMap(
        const std::unordered_map<std::string, const CANDBCSignalSchema>& signalNameToSignalMap);

    /// @brief Gets the vector of signal schemas.
    /// @return The vector of signal schemas.
    const std::vector<CANDBCSignalSchema>& getSignals() const;

    /// @brief Sets the vector of signal schemas.
    /// @param signals The new vector of signal schemas.
    void setSignals(const std::vector<CANDBCSignalSchema>& signals);

    CANDBCMessage createCANDBCMessage(std::unordered_map<std::string, double> signalValueMap) const;

private:
    std::string name_;                        ///< Name of the message.
    uint32_t address_;                        ///< Address of the message.
    uint32_t size_;                           ///< Size of the message in bytes.
    std::vector<CANDBCSignalSchema> signals_; ///< Vector of signal schemas.
    std::unordered_map<std::string, const CANDBCSignalSchema>
        signalNameToSignalMap_; ///< Map from signal names to signal schemas.
};

} // namespace platform::io::can::dbc
