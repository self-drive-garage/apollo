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

/// @file CANDBCMessageSchema.cpp
/// @brief This file contains the implementation of the CANDBCMessageSchema class.

#include "platform/io/can/dbc/CANDBCMessageSchema.hpp"

#include <utility>

namespace platform::io::can::dbc {

CANDBCMessageSchema::CANDBCMessageSchema(
    std::string name,
    const uint32_t address,
    const uint32_t size,
    const std::vector<CANDBCSignalSchema>& signals,
    const std::unordered_map<std::string, const CANDBCSignalSchema>& signal_name_to_signal_map)
    : name_(std::move(name))
    , address_(address)
    , size_(size)
    , signals_(signals)
    , signalNameToSignalMap_(signal_name_to_signal_map)
{}

uint32_t CANDBCMessageSchema::getAddress() const
{
    return address_;
}

void CANDBCMessageSchema::setAddress(const uint32_t address)
{
    address_ = address;
}

const std::string& CANDBCMessageSchema::getName() const
{
    return name_;
}

void CANDBCMessageSchema::setName(std::string name)
{
    name_ = std::move(name);
}

uint32_t CANDBCMessageSchema::getSize() const
{
    return size_;
}

void CANDBCMessageSchema::setSize(const uint32_t size)
{
    size_ = size;
}

std::optional<std::reference_wrapper<const CANDBCSignalSchema>> CANDBCMessageSchema::getSignalSchemaByName(
    const std::string& signalName) const
{
    const auto it = signalNameToSignalMap_.find(signalName);
    if (it != signalNameToSignalMap_.end()) {
        return std::cref(it->second);
    }
    return std::nullopt; // Represents an empty optional
}

const std::vector<CANDBCSignalSchema>& CANDBCMessageSchema::getSignals() const
{
    return signals_;
}

void CANDBCMessageSchema::setSignals(const std::vector<CANDBCSignalSchema>& signals)
{
    signals_ = signals;
}

void CANDBCMessageSchema::moveSignalSchemaToMap(std::string signalSchemaName, CANDBCSignalSchema signalSchema)
{
    signalNameToSignalMap_.emplace(std::move(signalSchemaName), std::move(signalSchema));
}

const std::unordered_map<std::string, const CANDBCSignalSchema>& CANDBCMessageSchema::getSignalNameToSignalMap() const
{
    return signalNameToSignalMap_;
}

void CANDBCMessageSchema::setSignalNameToSignalMap(
    const std::unordered_map<std::string, const CANDBCSignalSchema>& signalNameToSignalMap)
{
    CANDBCMessageSchema::signalNameToSignalMap_ = signalNameToSignalMap;
}

CANDBCMessage CANDBCMessageSchema::createCANDBCMessage(std::unordered_map<std::string, double> signalValueMap) const {
    CANDBCMessage message;
    message.name(name_);
    message.address(address_);

    for (const CANDBCSignalSchema& signalSchema : signals_) {
        message.addSignal(signalSchema.createCANDBCSignal(signalValueMap[signalSchema.getName()]));
    }

    return message;
}

} // namespace platform::io::can::dbc
