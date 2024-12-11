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
/// @brief This file contains the implementation of the CANDBC class and related structs and enums.
///
#include "platform/io/can/dbc/CANDBC.hpp"

#include "platform/io/can/dbc/CANDBCMessageSchema.hpp"
#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"
#include "platform/io/can/dbc/SubaruGlobalCANDBC.hpp"

#include <cstdint>
#include <cstring>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

namespace platform::io::can::dbc {

CANDBC::CANDBC(bool allow_duplicate_message_name)
{
    /// The variable `subaruGlobalCANDBC` is defined in header `SubaruGlobalCANDBC.hpp` which is generated
    /// by the bazel `genrule(subaru_global_can_dbc)` in the `BUILD.bazel.bazel` file.
    std::istringstream dbcStream(subaruGlobalCANDBC);

    uint32_t address = 0;
    std::set<uint32_t> addressSet;
    std::set<std::string> messageNameSet;
    std::map<uint32_t, std::set<std::string>> signalNameSetMap;
    std::map<uint32_t, std::vector<CANDBCSignalSchema>> signalAddressMap;
    std::setlocale(LC_NUMERIC, "C");

    // used to find big endian LSB from MSB and size
    std::vector<int> bigEndianBits;
    for (int i = 0; i < 64; i++) {
        for (int j = 7; j >= 0; j--) {
            bigEndianBits.push_back(j + i * 8);
        }
    }

    std::string line;
    int line_num = 0;
    std::smatch match;
    // TODO: see if we can speed up the regex statements in this loop, SG_ is specifically the slowest
    while (std::getline(dbcStream, line)) {
        line = trim(line);
        line_num += 1;
        if (startswith(line, "BO_ ")) {
            // new group
            bool ret = std::regex_match(line, match, boRegExp_);
            DBC_ASSERT(ret, "bad BO: " << line);

            address = std::stoul(match[1].str());
            auto result = messagesAddressMap_.emplace(address, CANDBCMessageSchema{});

            if (result.second) { // Check if insertion took place
                CANDBCMessageSchema& message = result.first->second;
                message.setAddress(std::stoul(match[1].str())); // could be hex
                message.setName(match[2].str());
                message.setSize(std::stoul(match[3].str()));

                // check for duplicates
                DBC_ASSERT(addressSet.find(address) == addressSet.end(),
                           "Duplicate message address: " << address << " (" << message.getName() << ")");
                addressSet.insert(address);

                if (!allow_duplicate_message_name) {
                    DBC_ASSERT(messageNameSet.find(message.getName()) == messageNameSet.end(),
                               "Duplicate message name: " << message.getName());
                    messageNameSet.insert(message.getName());
                }
            }
        } else if (startswith(line, "SG_ ")) {
            // new signal
            int offset = 0;
            if (!std::regex_search(line, match, sgRegExp_)) {
                bool ret = std::regex_search(line, match, sgmRegExp_);
                DBC_ASSERT(ret, "bad SG: " << line);
                offset = 1;
            }
            CANDBCSignalSchema& signalSchema = signalAddressMap[address].emplace_back();
            signalSchema.setName(match[1].str());
            signalSchema.setMessageName(messagesAddressMap_[address].getName());
            signalSchema.setStartBit(std::stoi(match[offset + 2].str()));
            signalSchema.setSize(std::stoi(match[offset + 3].str()));
            signalSchema.setIsLittleEndian(std::stoi(match[offset + 4].str()) == 1);
            signalSchema.setIsSigned(match[offset + 5].str() == "-");
            signalSchema.setFactor(std::stod(match[offset + 6].str()));
            signalSchema.setOffset(std::stod(match[offset + 7].str()));

            if (signalSchema.getName() == "CHECKSUM") {
                DBC_ASSERT(ChecksumState::checksumSize == -1 || signalSchema.getSize() == ChecksumState::checksumSize,
                           "CHECKSUM is not " << ChecksumState::checksumSize << " bits long");
                DBC_ASSERT(ChecksumState::checksumStartBit == -1 ||
                               (signalSchema.getStartBit() % 8) == ChecksumState::checksumStartBit,
                           " CHECKSUM starts at wrong bit");
                DBC_ASSERT(signalSchema.isLittleEndian() == ChecksumState::littleEndian,
                           "CHECKSUM has wrong endianness");
                signalSchema.setType(SignalType::CHECKSUM);
            } else if (signalSchema.getName() == "COUNTER") {
                DBC_ASSERT(ChecksumState::counterSize == -1 || signalSchema.getSize() == ChecksumState::counterSize,
                           "COUNTER is not " << ChecksumState::counterSize << " bits long");
                DBC_ASSERT(ChecksumState::counterStartBit == -1 ||
                               (signalSchema.getStartBit() % 8) == ChecksumState::counterStartBit,
                           "COUNTER starts at wrong bit");
                DBC_ASSERT(ChecksumState::littleEndian == signalSchema.isLittleEndian(),
                           "COUNTER has wrong endianness");
                signalSchema.setType(SignalType::COUNTER);
            }

            if (signalSchema.isLittleEndian()) {
                signalSchema.setLeastSignificantBit(signalSchema.getStartBit());
                signalSchema.setMostSignificantBit(signalSchema.getStartBit() + signalSchema.getSize() - 1);
            } else {
                auto it = std::find(bigEndianBits.begin(), bigEndianBits.end(), signalSchema.getStartBit());
                signalSchema.setLeastSignificantBit(
                    bigEndianBits[(it - bigEndianBits.begin()) + signalSchema.getSize() - 1]);
                signalSchema.setMostSignificantBit(signalSchema.getStartBit());
            }
            DBC_ASSERT(signalSchema.getLeastSignificantBit() < (64 * 8) &&
                           signalSchema.getMostSignificantBit() < (64 * 8),
                       "Signal out of bounds: " << line);

            // Check for duplicate signal names
            DBC_ASSERT(signalNameSetMap[address].find(signalSchema.getName()) == signalNameSetMap[address].end(),
                       "Duplicate signal name: " << signalSchema.getName());
            signalNameSetMap[address].insert(signalSchema.getName());

            // now we store a const reference to the SignalSchema in a name to signal map
            // for quick lookup during CAN Send/Receive operations
            std::string signalSchemaName = signalSchema.getName();
            CANDBCSignalSchema signalSchema1 = signalSchema;
            //                auto& signalNameToSignalMap = messagesAddressMap_[address].getSignalNameToSignalMap();
            //                signalNameToSignalMap.emplace(std::move(signalSchemaName), std::move(signalSchema1));
            messagesAddressMap_[address].moveSignalSchemaToMap(std::move(signalSchemaName), std::move(signalSchema1));

        } else if (startswith(line, "VAL_ ")) {
            // new signal value/definition
            bool ret = std::regex_search(line, match, valRegExp_);
            DBC_ASSERT(ret, "bad VAL: " << line);

            address = std::stoul(match[1].str());
            auto name = match[2].str();
            auto valueDefinitions = match[3].str();
            std::sregex_token_iterator it{valueDefinitions.begin(), valueDefinitions.end(), valSplitRegExp_, -1};
            // convert strings to UPPER_CASE_WITH_UNDERSCORES
            std::vector<std::string> words{it, {}};
            for (auto& w : words) {
                w = trim(w);
                std::transform(w.begin(), w.end(), w.begin(), ::toupper);
                std::replace(w.begin(), w.end(), ' ', '_');
            }
            // join string
            std::stringstream s;
            std::copy(words.begin(), words.end(), std::ostream_iterator<std::string>(s, " "));
            valueDefinitions = s.str();
            valueDefinitions = trim(valueDefinitions);

            auto& signals = signalAddressMap[address];
            for (auto signal : signals) {
                if (signal.getName() == name) {
                    signal.setValueDescription(CANDBCSignalSchema::ValueDescription{name, address, valueDefinitions});
                }
            }
        }
    }
    // we now create a map to find messages by name as well as ID
    for (auto& [key, message] : messagesAddressMap_) {
        message.setSignals(signalAddressMap[key]);
        messagesNameMap_[message.getName()] = message;
    }
}

std::optional<std::reference_wrapper<const CANDBCMessageSchema>> CANDBC::getMessageByAddress(uint32_t address)
{
    const auto it = messagesAddressMap_.find(address);
    if (it != messagesAddressMap_.end()) {
        return std::cref(it->second);
    } else {
        return std::nullopt; // Represents an empty optional
    }
}

std::optional<std::reference_wrapper<const CANDBCMessageSchema>> CANDBC::getMessageByName(std::string messageName)
{
    const auto it = messagesNameMap_.find(messageName);
    if (it != messagesNameMap_.end()) {
        return std::cref(it->second);
    } else {
        return std::nullopt; // Represents an empty optional
    }
}

std::optional<std::reference_wrapper<const std::vector<CANDBCSignalSchema>>> CANDBC::getSignalSchemasByAddress(
    uint32_t address)
{
    const auto it = messagesAddressMap_.find(address);
    if (it != messagesAddressMap_.end()) {
        return it->second.getSignals();
    } else {
        return std::nullopt; // Represents an empty optional
    }
}

std::string& CANDBC::trim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s.erase(0, s.find_first_not_of(t));
}

bool CANDBC::startswith(const std::string& str, const char* prefix)
{
    return str.find(prefix, 0) == 0;
}

bool CANDBC::startswith(const std::string& str, std::initializer_list<const char*> prefix_list)
{
    for (auto prefix : prefix_list) {
        if (startswith(str, prefix))
            return true;
    }
    return false;
}

bool CANDBC::endswith(const std::string& str, const char* suffix)
{
    return str.find(suffix, 0) == (str.length() - strlen(suffix));
}

size_t CANDBC::bufferSizeToDataLengthCode(uint8_t len)
{
    auto result = std::find(std::begin(dataLengthCodeToNumBytes), std::end(dataLengthCodeToNumBytes), len);
    if (result != std::end(dataLengthCodeToNumBytes)) {
        int index = result - std::begin(dataLengthCodeToNumBytes);
        return index;
    }
    // TODO: return something that makes more sense
    return 255;
}

} // namespace platform::io::can::dbc
