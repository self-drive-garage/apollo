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

/// @file SocketCANDevice.hpp
/// @brief Defines the SocketCANDevice class for handling CAN communication using sockets.
/// @details This class provides methods for initializing a CAN device, sending and receiving CAN messages.

#pragma once

#include "platform/io/sockets/ISocketOps.hpp"
#include "platform/io/sockets/SocketOps.hpp"
#include "platform/io/Status.hpp"
#include "platform/io/can/SocketCANMessage.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace platform::io::can {

/// @class SocketCANDevice
/// @brief Class for handling CAN communication using sockets.
/// @details This class provides methods to initialize the CAN device, send messages, and receive messages.
class SocketCANDevice
{
public:
    /// @brief Constructs a SocketCANDevice object.
    /// @param socketOps A shared pointer to an ISocketOps object for socket operations. Defaults to a new SocketOps
    /// @param isIsoTpSocket A boolean specifying whether this socket should use the ISO-TP CAN protocol
    /// @param rxId The CAN ID for receiving responses from the ECU.
    /// @param txId The CAN ID for sending requests to the ECU.
    /// instance.
    explicit SocketCANDevice(std::string interfaceName = std::string("can0"),
                             std::shared_ptr<ISocketOps> socketOps = std::make_shared<SocketOps>());

    /// @brief Receives a CAN message from the socket.
    /// @return A tuple containing the Status and an optional SocketCANMessage.
    std::tuple<Status, std::optional<SocketCANMessage>> getMessage();

    /// @brief Sends a CAN message to the specified address with raw data.
    /// @param address The CAN address to send the message to.
    /// @param rawData The raw data to be sent in the CAN message.
    /// @return Status object indicating success or error.
    Status sendMessage(uint32_t address, const std::vector<uint8_t>& rawData) const;

    /// @brief Sends a CAN message.
    /// @param messageToSend The SocketCANMessage to send.
    /// @return Status object indicating success or error.
    Status sendMessage(const SocketCANMessage& messageToSend) const;

    /// @brief Sends a CAN frame.
    /// @param frameToSend The CAN frame to send.
    /// @return Status object indicating success or error.
    Status sendMessage(const can_frame& frameToSend) const;

    Status disableECU(int retry = 10, double timeout = 0.1);

  Status close();

private:
    /// @brief Initializes the CAN device.
    /// @return Status object indicating success or error.
    Status initDevice();

    /// @brief Initializes the CAN device for ISO-TP communication.
    /// @details This function configures the CAN device to communicate with an ECU
    ///          using the ISO-TP (ISO 15765-2) protocol. It sets up the device with
    ///          specific transmit (TX) and receive (RX) CAN IDs required for
    ///          diagnostic communication.
    /// @param rxId The CAN ID for receiving responses from the ECU.
    /// @param txId The CAN ID for sending requests to the ECU.
    /// @return Status object indicating success or error.
    Status initIsoTpDevice(uint32_t rxId, uint32_t txId);

    /// @brief Reads data from the socket.
    /// @param buf The buffer to read data into.
    /// @return Status object indicating success or error.
    Status readSocket(std::vector<uint8_t>& buf) const;

    /// @brief Writes data to the socket.
    /// @param buf The buffer containing the data to write.
    /// @param bytesWritten The number of bytes written.
    /// @return Status object indicating success or error.
    Status writeToSocket(const std::vector<uint8_t>& buf, ssize_t& bytesWritten) const;

    /// @brief Closes the socket.
    /// @param fd The file descriptor of the socket to close.
    /// @return Status object indicating success or error.
    Status closeSocket(int fd) const;

    std::mutex mutex_;                      ///< Mutex to protect socket operations.
    int32_t socketFd_{0};                   ///< The CAN socket file descriptor.
    std::string interfaceName_;             ///< The CAN interface name.
    std::shared_ptr<ISocketOps> socketOps_; ///< Shared pointer to the ISocketOps object.

    uint32_t rxId_ = 0x78F; ///< Response ID from the ECU for ISO-TP CAN protocol
    uint32_t txId_ = 0x787; ///<  Request ID to the ECU for ISO-TP CAN protocol
};

} // namespace platform::io::can
