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

/// @file CANDBCSignalSchema.cpp
/// @brief This file contains the implementation of the CANDBCSignalSchema class.
#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"

namespace platform::io::can::dbc {

CANDBCSignalSchema::CANDBCSignalSchema(const std::string& name,
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
                                       const ValueDescription& value_description)
    : name_(name)
    , messageName_(message_name)
    , startBit_(start_bit)
    , mostSignificantBit_(most_significant_bit)
    , leastSignificantBit_(least_significant_bit)
    , size_(size)
    , isSigned_(is_signed)
    , factor_(factor)
    , offset_(offset)
    , isLittleEndian_(is_little_endian)
    , type_(type)
    , valueDescription_(value_description)
{}

/// This is taken from CommaAI openpilot
/// reference:
/// https://github.com/commaai/opendbc/blob/2b96bcc45669cdd14f9c652b07ef32d6403630f6/can/common.cpp#L27C1-L27C1
uint8_t CANDBCSignalSchema::calcSubaruChecksum(uint32_t address, const std::vector<uint8_t>& d)
{
    unsigned int s = 0;
    while (address) {
        s += address & 0xFF;
        address >>= 8;
    }
    // skip checksum in first byte
    for (size_t i = 1; i < d.size(); i++) {
        s += d[i];
    }
    return s & 0xFF;
};

int64_t CANDBCSignalSchema::parseValue(const std::vector<uint8_t>& messageData) const
{
    int64_t ret = 0;
    int i = mostSignificantBit_ / 8;
    int bits = size_;
    while (i >= 0 && i < static_cast<int>(messageData.size()) && bits > 0) {
        int lsb = (int)(leastSignificantBit_ / 8) == i ? leastSignificantBit_ : i * 8;
        int msb = (int)(mostSignificantBit_ / 8) == i ? mostSignificantBit_ : (i + 1) * 8 - 1;
        int size = msb - lsb + 1;

        uint64_t d = (messageData[i] >> (lsb - (i * 8))) & ((1ULL << size) - 1);
        ret |= d << (bits - size);

        bits -= size;
        i = isLittleEndian() ? i - 1 : i + 1;
    }
    return ret;
}

// void CANDBCSignalSchema::packValue(std::vector<uint8_t>& writeBuffer, int64_t rawValue) const
// {
//     int i = leastSignificantBit_ / 8;
//     int bits = size_;
//     if (size_ < 64) {
//         rawValue &= ((1ULL << size_) - 1);
//     }
//
//     while (i >= 0 && i < static_cast<int>(writeBuffer.size()) && bits > 0) {
//         int shift = (int)(leastSignificantBit_ / 8) == i ? leastSignificantBit_ % 8 : 0;
//         int size = std::min(bits, 8 - shift);
//
//         writeBuffer[i] &= ~(((1ULL << size) - 1) << shift);
//         writeBuffer[i] |= (rawValue & ((1ULL << size) - 1)) << shift;
//
//         bits -= size;
//         rawValue >>= size;
//         i = isLittleEndian() ? i + 1 : i - 1;
//     }
// }

CANDBCSignal CANDBCSignalSchema::createCANDBCSignal(const double value) const {
    return CANDBCSignal(get_now_as_nanoseconds(), name_ , value, isLittleEndian());
}

void CANDBCSignalSchema::packValue(std::vector<uint8_t>& writeBuffer, int64_t rawValue) const
{
    int byteIndex = leastSignificantBit_ / 8;
    int bitsToWrite = size_;

    if (size_ < 64) {
        rawValue &= ((1ULL << size_) - 1);
    }

    while (byteIndex >= 0 && byteIndex < static_cast<int>(writeBuffer.size()) && bitsToWrite > 0) {
        int bitShift = (leastSignificantBit_ / 8 == byteIndex) ? leastSignificantBit_ % 8 : 0;
        int bitsInCurrentByte = std::min(bitsToWrite, 8 - bitShift);

        uint8_t mask = ((1ULL << bitsInCurrentByte) - 1) << bitShift;
        writeBuffer[byteIndex] = (writeBuffer[byteIndex] & ~mask) | ((rawValue << bitShift) & mask);

        bitsToWrite -= bitsInCurrentByte;
        rawValue >>= bitsInCurrentByte;
        byteIndex += isLittleEndian() ? 1 : -1;
    }
}

const std::string& CANDBCSignalSchema::getName() const
{
    return name_;
}

void CANDBCSignalSchema::setName(std::string name)
{
    name_ = name;
}

const std::string& CANDBCSignalSchema::getMessageName() const
{
    return messageName_;
}

int CANDBCSignalSchema::getStartBit() const
{
    return startBit_;
}

int CANDBCSignalSchema::getMostSignificantBit() const
{
    return mostSignificantBit_;
}

int CANDBCSignalSchema::getLeastSignificantBit() const
{
    return leastSignificantBit_;
}

int CANDBCSignalSchema::getSize() const
{
    return size_;
}
bool CANDBCSignalSchema::isSigned() const
{
    return isSigned_;
}

double CANDBCSignalSchema::getFactor() const
{
    return factor_;
}
double CANDBCSignalSchema::getOffset() const
{
    return offset_;
}

bool CANDBCSignalSchema::isLittleEndian() const
{
    return isLittleEndian_;
}

const SignalType& CANDBCSignalSchema::getSignalType() const
{
    return type_;
}

void CANDBCSignalSchema::setMessageName(const std::string& messageName)
{
    messageName_ = messageName;
}

void CANDBCSignalSchema::setStartBit(int startBit)
{
    startBit_ = startBit;
}

void CANDBCSignalSchema::setMostSignificantBit(int mostSignificantBit)
{
    mostSignificantBit_ = mostSignificantBit;
}

void CANDBCSignalSchema::setLeastSignificantBit(int leastSignificantBit)
{
    leastSignificantBit_ = leastSignificantBit;
}

void CANDBCSignalSchema::setSize(int size)
{
    size_ = size;
}

void CANDBCSignalSchema::setIsSigned(bool isSigned)
{
    isSigned_ = isSigned;
}

void CANDBCSignalSchema::setFactor(double factor)
{
    factor_ = factor;
}

void CANDBCSignalSchema::setOffset(double offset)
{
    offset_ = offset;
}

void CANDBCSignalSchema::setIsLittleEndian(bool isLittleEndian)
{
    isLittleEndian_ = isLittleEndian;
}

SignalType CANDBCSignalSchema::getType() const
{
    return type_;
}

void CANDBCSignalSchema::setType(SignalType type)
{
    type_ = type;
}

const CANDBCSignalSchema::ValueDescription& CANDBCSignalSchema::getValueDescription() const
{
    return valueDescription_;
}

void CANDBCSignalSchema::setValueDescription(const CANDBCSignalSchema::ValueDescription& valueDescription)
{
    valueDescription_ = valueDescription;
}

} // namespace platform::io::can::dbc
