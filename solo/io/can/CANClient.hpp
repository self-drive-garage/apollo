
/// @file CANClient.hpp
/// @brief This file contains the definition of the CANClient. A client for handling CAN communication.
/// @details This class provides functionality for interacting with a CAN device and processing CAN messages.
/// It uses a CAN device and CAN database to send and receive messages.

#pragma once

#include "platform/io/Status.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"
#include "platform/io/can/dbc/CANDBCMessage.hpp"
#include "platform/io/can/dbc/CANDBCMessageSchema.hpp"

#include <linux/can.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <units.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace platform::io::can {

/// @class CANClient
/// @brief Client for handling CAN communication.
///
/// @tparam CANDevice The type of CAN device.
template<class CANDevice>
class CANClient
{
public:
    /// @brief Constructor.
    /// @param canDevice The CAN device.
    /// @param canDatabase The CAN database.
    CANClient(std::unique_ptr<CANDevice> canDevice, std::unique_ptr<dbc::CANDBC> canDatabase);

    /// @brief Waits for CAN messages to appear.
    /// @param timeout The timeout period to wait for messages.
    /// @return True if messages appear, false otherwise.
    // bool waitForMessages(units::time::millisecond_t timeout = units::time::millisecond_t(3000));

    /// @brief Begin reading and buffer messages from the CAN Device.
    void enQueueMessages();

    /// @brief Attempts to send a single CAN message.
    /// @param message The CAN message to send.
    /// @return Status::SUCCESS if writing is successful in addition to the raw
    /// binary data generated from the message schema.  Status::ERROR and std::nullopt otherwise
    template<class CANDeviceMessage>
    std::tuple<Status, std::optional<std::vector<uint8_t>>> sendMessage(const dbc::CANDBCMessage& message);

    /// @brief Sends a CAN message to the specified address with raw data.
    /// @param address The CAN address to send the message to.
    /// @param rawData The raw data to be sent in the CAN message.
    /// @return Status object indicating success or error.
    template<class CANDeviceMessage>
    Status sendRawMessage(uint32_t address, std::vector<uint8_t> rawData) const;

    /// @brief Receives a CAN message from the socket.
    /// @return A tuple containing the Status and an optional SocketCANMessage.
    template<class CANDeviceMessage>
    std::tuple<Status, std::optional<CANDeviceMessage>> listenForMessageByAddress(uint32_t messageAddress,
                                                                                  std::chrono::milliseconds timeout);

    /// @brief Returns all queued messages and clears the Queue.
    /// @return A vector of all queued messages.
    std::vector<dbc::CANDBCMessage> getQueuedMessagesAndClearQueue();

    /// @brief Returns all queued raw data and clears the Queue.
    /// @return A vector of all queued raw data.
    std::vector<can_frame> getRawDataAndClearQueue();

    std::optional<dbc::CANDBCMessage> createCANDBCMessage(
        const std::string& name,
        const std::unordered_map<std::string, double>& signalValueMap) const;

private:
    static uint64_t getCurrentTimeInNanoSeconds()
    {
        auto now = std::chrono::steady_clock::now();
        auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
        auto epoch = now_ns.time_since_epoch();
        return epoch.count();
    }

    /// @brief Attempts to read a single message from the bus.
    template<class CANDeviceMessage>
    CANDeviceMessage getMessage();

    /// @brief Queues a message to the internal buffer.
    /// @param message The CAN message to queue.
    void queueMessage(const dbc::CANDBCMessage& message);

    mutable std::mutex qMutex_;                   ///< Mutex for thread-safe access to the queue.
    std::unique_ptr<CANDevice> canDevice_;        ///< The CAN device.
    std::unique_ptr<dbc::CANDBC> canDatabase_;    ///< The CAN database.
    std::vector<dbc::CANDBCMessage> canMessages_; ///< Buffer for CAN messages.
    std::vector<can_frame> rawData_;
    std::unordered_map<uint32_t, uint32_t> messageCountsByAddress_; ///< Map to store message counts by address.

    static constexpr bool ignoreChecksum = true; ///< Flag to ignore checksum validation.
    static constexpr bool ignoreCounter = false; ///< Flag to ignore counter validation.
};

template<class CANDevice>
CANClient<CANDevice>::CANClient(std::unique_ptr<CANDevice> canDevice, std::unique_ptr<dbc::CANDBC> canDatabase)
    : canDevice_{std::move(canDevice)}
    , canDatabase_{std::move(canDatabase)}
{
    ADEBUG << "CANClient initialized with CANDevice and CANDatabase");
}

template<class CANDevice>
template<class CANDeviceMessage>
CANDeviceMessage CANClient<CANDevice>::getMessage()
{
    auto [status, message] = canDevice_->getMessage();
    return message.value();
}

template<class CANDevice>
template<class CANDeviceMessage>
Status CANClient<CANDevice>::sendRawMessage(uint32_t address, const std::vector<uint8_t> rawData) const
{
    CANDeviceMessage messageToSend = CANDeviceMessage::fromRawData(address, std::move(rawData));
    return canDevice_->sendMessage(messageToSend);
}

template<class CANDevice>
template<class CANDeviceMessage>
std::tuple<Status, std::optional<CANDeviceMessage>> CANClient<CANDevice>::listenForMessageByAddress(
    uint32_t messageAddress,
    const std::chrono::milliseconds timeout)
{
    auto start = std::chrono::steady_clock::now();
    while (true) {

        if (auto message = getMessage<CANDeviceMessage>(); message.getAddress() == messageAddress) {
            return {{STATUS::SUCCESS}, std::move(message)};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (auto end = std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start) > timeout) {
            return {{STATUS::ERROR}, std::nullopt};
        }
    }
}

template<class CANDevice>
void CANClient<CANDevice>::enQueueMessages()
{
    if (auto [status, canDeviceMessageOpt] = canDevice_->getMessage();
        status.ok() && canDeviceMessageOpt.has_value()) {
        const auto& canDeviceMessage = canDeviceMessageOpt.value();
        const auto& rawData = canDeviceMessage.getRawFrame();
        rawData_.emplace_back(rawData);
        dbc::CANDBCMessage canMessage{canDeviceMessage.getAddress()};

        if (auto const& signalsRef = canDatabase_->getSignalSchemasByAddress(canDeviceMessage.getAddress());
            signalsRef.has_value()) {
            auto const& signalSchemas = signalsRef.value().get();

            std::vector<dbc::CANDBCSignal> canSignals{};
            canSignals.reserve(signalSchemas.size());

            canMessage.name(signalSchemas[0].getMessageName());

            for (dbc::CANDBCSignalSchema const& signalSchema : signalSchemas) {
                int64_t tmp = signalSchema.parseValue(canDeviceMessage.getFrameData());

                if (signalSchema.isSigned()) {
                    tmp -= ((tmp >> (signalSchema.getSize() - 1)) & 0x1) ? (1ULL << signalSchema.getSize()) : 0;
                }
                bool checksum_failed = false;
                if (!ignoreChecksum) {
                    if (signalSchema.calcSubaruChecksum(canDeviceMessage.getAddress(),
                                                        canDeviceMessage.getFrameData()) != tmp) {
                        checksum_failed = true;
                    }
                }
                bool counter_failed = false;

                if (checksum_failed || counter_failed) {
                    AWARN << "Message checks failed for address: {} checksum_failed: {} counter_failed: {}",
                                 canDeviceMessage.getAddress(),
                                 checksum_failed,
                                 counter_failed);
                    continue;
                }
                canSignals.emplace_back(getCurrentTimeInNanoSeconds(),
                                        signalSchema.getName(),
                                        tmp * signalSchema.getFactor() + signalSchema.getOffset(),
                                        signalSchema.isLittleEndian());
            }
            canMessage.signals(std::move(canSignals));
            queueMessage(std::move(canMessage));
            ADEBUG << "Enqueued CAN message with address: {}", canDeviceMessage.getAddress());
        }
    }
}

template<class CANDevice>
std::vector<dbc::CANDBCMessage> CANClient<CANDevice>::getQueuedMessagesAndClearQueue()
{
    std::lock_guard<std::mutex> lock(qMutex_);
    std::vector<dbc::CANDBCMessage> currentMessages = std::move(canMessages_);
    ADEBUG << "Retrieved and cleared {} queued CAN messages", currentMessages.size());
    return currentMessages;
}

template<class CANDevice>
std::vector<can_frame> CANClient<CANDevice>::getRawDataAndClearQueue()
{
    std::lock_guard<std::mutex> lock(qMutex_);
    std::vector<can_frame> currentMessages = std::move(rawData_);
    ADEBUG << "Retrieved and cleared {} queued raw CAN data", currentMessages.size());
    return currentMessages;
}

template<class CANDevice>
void CANClient<CANDevice>::queueMessage(const dbc::CANDBCMessage& message)
{
    std::lock_guard<std::mutex> lock(qMutex_);
    canMessages_.emplace_back(message);
}

template<class CANDevice>
template<class CANDeviceMessage>
std::tuple<Status, std::optional<std::vector<uint8_t>>> CANClient<CANDevice>::sendMessage(
    const dbc::CANDBCMessage& message)
{
    if (const auto messageSchemaRef = canDatabase_->getMessageByName(message.name()); messageSchemaRef.has_value()) {
        bool setCounterSignal = false;
        const auto& messageSchema = messageSchemaRef.value().get();
        std::vector<uint8_t> rawData(messageSchema.getSize(), 0);

        for (const auto& messageSignal : message.signals()) {
            if (const auto signalSchemaRef = messageSchema.getSignalSchemaByName(messageSignal.name());
                signalSchemaRef.has_value()) {
                const auto signalSchema = signalSchemaRef.value().get();
                auto rawValue = static_cast<int64_t>(
                    std::round((messageSignal.value() - signalSchema.getOffset()) / signalSchema.getFactor()));
                if (rawValue < 0) {
                    rawValue = (1ULL << signalSchema.getSize()) + rawValue;
                }
                signalSchema.packValue(rawData, rawValue);

                setCounterSignal = setCounterSignal || (signalSchema.getName() == "COUNTER");
                if (setCounterSignal) {
                    messageCountsByAddress_[message.address()] = messageSignal.value();
                }
            }
        }

        if (const auto countSignalSchemaRef = messageSchema.getSignalSchemaByName("COUNT");
            !setCounterSignal && countSignalSchemaRef.has_value()) {
            const auto& countSignalSchema = countSignalSchemaRef.value().get();

            if (!messageCountsByAddress_.contains(message.address())) {
                messageCountsByAddress_[message.address()] = 0;
            }
            countSignalSchema.packValue(rawData, messageCountsByAddress_[message.address()]);
            messageCountsByAddress_[message.address()] =
                (messageCountsByAddress_[message.address()] + 1) % (1 << countSignalSchema.getSize());
        }

        if (const auto checksumSignalSchemaRef = messageSchema.getSignalSchemaByName("CHECKSUM");
            checksumSignalSchemaRef.has_value()) {
            const auto& checksumSignalSchema = checksumSignalSchemaRef.value().get();
            const unsigned int checksum =
                platform::io::can::dbc::CANDBCSignalSchema::calcSubaruChecksum(message.address(), rawData);
            checksumSignalSchema.packValue(rawData, checksum);
        }

        const size_t dataLengthCode = dbc::CANDBC::bufferSizeToDataLengthCode(rawData.size());

        assert(rawData.size() <= 64);
        assert(rawData.size() == dbc::CANDBC::dataLengthCodeToNumBytes[dataLengthCode]);

        CANDeviceMessage messageToSend = CANDeviceMessage::fromCANDBCMessage(message, rawData);

        if (Status status = canDevice_->sendMessage(messageToSend); status.isError()) {
            AERROR << "Failed to send CAN message with address: {}", message.address());
            return {{STATUS::ERROR}, rawData};
        }
        ADEBUG << "Successfully sent CAN message with address: {}", message.address());
        return {{STATUS::SUCCESS}, rawData};
    }
    AERROR << "Message schema for name: {} not found", message.name());
    return {{STATUS::ERROR}, std::nullopt};
}

template<class CANDevice>
std::optional<dbc::CANDBCMessage> CANClient<CANDevice>::createCANDBCMessage(
    const std::string& name,
    const std::unordered_map<std::string, double>& signalValueMap) const
{
    if (const auto messageSchemaRef = canDatabase_->getMessageByName(name); messageSchemaRef.has_value()) {
        const auto& messageSchema = messageSchemaRef.value().get();
        return messageSchema.createCANDBCMessage(signalValueMap);
    }
    return std::nullopt;
}

} // namespace platform::io::can
