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

/// @file SocketCANDevice.cpp
/// @brief Implementatinof of the SocketCANDevice class for handling CAN communication using sockets.
/// @details This class provides methods for initializing a CAN device, sending and receiving CAN messages.

#include "platform/io/can/SocketCANDevice.hpp"

#include "platform/io/can/exceptions/CANExceptions.hpp"

#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/isotp.h>
#include <net/if.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace platform::io::can {

SocketCANDevice::SocketCANDevice(std::string interfaceName, std::shared_ptr<ISocketOps> socketOps)
    : interfaceName_{std::move(interfaceName)}
    , socketOps_(std::move(socketOps))
{
    ADEBUG << "Initializing SocketCANDevice on interface {}", interfaceName_);
    if (const Status status = initDevice(); status.isError()) {
        spdlog::critical("Failed to initialize SocketCANDevice: {}", status.toString());
        throw exceptions::CANInitException(status.toString());
    }
    ADEBUG << "SocketCANDevice initialized successfully");
}

std::tuple<Status, std::optional<SocketCANMessage>> SocketCANDevice::getMessage()
{
    std::lock_guard<std::mutex> lock(mutex_);
    can_frame canFrame{};
    std::vector<uint8_t> buffer(sizeof(canFrame));
    Status status = readSocket(buffer);
    if (status.isError()) {
        AERROR << "Failed to read CAN message: {}", status.toString());
        return {status, std::nullopt};
    }
    std::memcpy(&canFrame, buffer.data(), sizeof(canFrame));
    ADEBUG << "Received CAN message with ID: {}", canFrame.can_id);
    return {status, SocketCANMessage{canFrame}};
}

Status SocketCANDevice::sendMessage(const can_frame& frameToSend) const
{
    const std::vector<uint8_t> buffer(reinterpret_cast<const uint8_t*>(&frameToSend),
                                      reinterpret_cast<const uint8_t*>(&frameToSend) + sizeof(frameToSend));
    ssize_t bytesWritten = 0;
    Status status = writeToSocket(buffer, bytesWritten);
    if (status.isError()) {
        AERROR << "Failed to send CAN message with ID: {}: {}", frameToSend.can_id, status.toString());
    } else {
        ADEBUG << "Sent CAN message with ID: {}", frameToSend.can_id);
    }
    return status;
}

Status SocketCANDevice::sendMessage(uint32_t address, const std::vector<uint8_t>& rawData) const
{
    can_frame frameToSend{};
    frameToSend.can_id = address;
    // std::ranges::copy(rawData, frameToSend.data);

    if (rawData.size() > 8) {
        AERROR << "Raw data exceeds maximum CAN frame data size.");
        return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, "Raw data too large"};
    }

    std::copy(rawData.begin(), rawData.end(), frameToSend.data);
    std::fill(frameToSend.data + rawData.size(), frameToSend.data + 8, 0); // Ensure remaining bytes are zero

    return sendMessage(frameToSend);
}

Status SocketCANDevice::sendMessage(const SocketCANMessage& messageToSend) const
{
    return sendMessage(messageToSend.getRawFrame());
}

Status SocketCANDevice::initDevice()
{
    sockaddr_can address;
    ifreq ifaceRequest;
    int64_t fdOptions = 0;

    memset(&address, 0, sizeof(address));
    memset(&ifaceRequest, 0, sizeof(ifaceRequest));

    auto result = socketOps_->socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (result.first.isError()) {
        AERROR << "Failed to create socket: {}", result.first.toString());
        return result.first;
    }

    socketFd_ = result.second.value();
    if (socketFd_ == -1) {
        spdlog::critical("Failed to initialize socketcan!");
        return {STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, "FAILED to initialise socketcan!", errno};
    }

    strcpy(ifaceRequest.ifr_name, interfaceName_.c_str());

    Status status = socketOps_->ioctl(socketFd_, SIOCGIFINDEX, ifaceRequest);
    if (status.isError()) {
        AERROR << "Failed to perform IO control operation on socket {}: {}", interfaceName_, status.toString());
        return {STATUS::ERROR,
                ERROR::COMMUNICATION_ERROR,
                "FAILED to perform IO control operation on socket " + interfaceName_ +
                    "! Errors: " + std::strerror(errno),
                errno};
    }

    status = socketOps_->fcntl(socketFd_, F_GETFL);
    if (status.isError()) {
        AERROR << "Failed to get file control flags: {}", status.toString());
        return status;
    }

    fdOptions |= O_NONBLOCK;
    status = socketOps_->fcntl(socketFd_, F_SETFL, fdOptions);
    if (status.isError()) {
        AERROR << "Failed to set file control flags: {}", status.toString());
        return status;
    }

    address.can_family = PF_CAN;
    address.can_ifindex = ifaceRequest.ifr_ifindex;

    auto addressPtr = std::make_shared<struct sockaddr_can>(address);
    status = socketOps_->bind(socketFd_, addressPtr, sizeof(address));
    if (status.isError()) {
        AERROR << "Failed to bind to socket CAN: {}", status.toString());
        return {STATUS::ERROR,
                ERROR::ERROR_CREATING_DEVICE,
                "FAILED to bind to socket CAN! Errors: " + std::string(std::strerror(errno)),
                errno};
    }

    ADEBUG << "SocketCANDevice initialized on interface {}", interfaceName_);
    return Status::OK();
}

// Status SocketCANDevice::initIsoTpDevice(const uint32_t rxId, const uint32_t txId)
// {
//     rxId_ = rxId;
//     txId_ = txId;
//
//     sockaddr_can address;
//     ifreq ifaceRequest;
//     auto opts = std::make_shared<can_isotp_options>();
//
//     memset(&address, 0, sizeof(address));
//     memset(&ifaceRequest, 0, sizeof(ifaceRequest));
//
//     auto result = socketOps_->socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP);
//     if (result.first.isError()) {
//         AERROR << "Failed to create ISO-TP socket: {}", result.first.toString());
//         return result.first;
//     }
//
//     socketFd_ = result.second.value();
//     if (socketFd_ == -1) {
//         spdlog::critical("Failed to initialize ISO-TP socket!");
//         return {STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, "FAILED to initialise ISO-TP socket!", errno};
//     }
//
//     strcpy(ifaceRequest.ifr_name, interfaceName_.c_str());
//
//     Status status = socketOps_->ioctl(socketFd_, SIOCGIFINDEX, ifaceRequest);
//     if (status.isError()) {
//         AERROR <<
//             "Failed to perform IO control operation on ISO-TP socket {}: {}", interfaceName_, status.toString());
//         return {STATUS::ERROR,
//                 ERROR::COMMUNICATION_ERROR,
//                 "FAILED to perform IO control operation on ISO-TP socket " + interfaceName_ +
//                     "! Errors: " + std::strerror(errno),
//                 errno};
//     }
//
//     address.can_family = PF_CAN;
//     address.can_ifindex = ifaceRequest.ifr_ifindex;
//     address.can_addr.tp.rx_id = rxId_; // Receiver CAN ID
//     address.can_addr.tp.tx_id = txId_; // Transmitter CAN ID
//
//     auto addressPtr = std::make_shared<struct sockaddr_can>(address);
//     status = socketOps_->bind(socketFd_, addressPtr, sizeof(address));
//     if (status.isError()) {
//         AERROR << "Failed to bind to ISO-TP socket: {}", status.toString());
//         return {STATUS::ERROR,
//                 ERROR::ERROR_CREATING_DEVICE,
//                 "FAILED to bind to ISO-TP socket! Errors: " + std::string(std::strerror(errno)),
//                 errno};
//     }
//
//     // Optional: set CAN ISO-TP options
//     status = socketOps_->setsockopt(socketFd_, SOL_CAN_ISOTP, CAN_ISOTP_OPTS, opts);
//
//     if (status.isError()) {
//         AERROR << "Failed to set ISO-TP Options: {}", status.toString());
//         socketOps_->close(socketFd_);
//         return {STATUS::ERROR,
//                 ERROR::ERROR_CREATING_DEVICE,
//                 "Failed to set ISO-TP Options! Errors: " + std::string(std::strerror(errno)),
//                 errno};
//     }
//
//     ADEBUG <<
//         "ISO-TP SocketCANDevice initialized on interface {} with RX ID: {}, TX ID: {}", interfaceName_, rxId_,
//         txId_);
//     return Status::OK();
// }

Status SocketCANDevice::initIsoTpDevice(const uint32_t rxId, const uint32_t txId)
{
    rxId_ = rxId;
    txId_ = txId;

    sockaddr_can address;
    ifreq ifaceRequest;
    auto opts = std::make_shared<can_isotp_options>();

    memset(&address, 0, sizeof(address));
    memset(&ifaceRequest, 0, sizeof(ifaceRequest));

    AERROR << ">>>>>> using txId = {} and rxId = {}", txId_, rxId_);

    auto result = socketOps_->socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP);
    if (result.first.isError()) {
        AERROR << "Failed to create ISO-TP socket: {}", result.first.toString());
        return result.first;
    }

    socketFd_ = result.second.value();
    if (socketFd_ == -1) {
        spdlog::critical("Failed to initialize ISO-TP socket!");
        return {STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, "FAILED to initialize ISO-TP socket!", errno};
    }

    // Set ISO-TP options before binding the socket
    memset(opts.get(), 0, sizeof(can_isotp_options));
    opts->flags |= CAN_ISOTP_FORCE_TXSTMIN; // Optional: force transmission spacing

    Status status = socketOps_->setsockopt(socketFd_, SOL_CAN_ISOTP, CAN_ISOTP_OPTS, opts);
    if (status.isError()) {
        AERROR << "Failed to set ISO-TP Options: {}", status.toString());
        socketOps_->close(socketFd_);
        return {STATUS::ERROR,
                ERROR::ERROR_CREATING_DEVICE,
                "Failed to set ISO-TP Options! Errors: " + std::string(std::strerror(errno)),
                errno};
    }

    // Get interface index and bind the socket to the CAN interface
    strcpy(ifaceRequest.ifr_name, interfaceName_.c_str());

    status = socketOps_->ioctl(socketFd_, SIOCGIFINDEX, ifaceRequest);
    if (status.isError()) {
        AERROR <<
            "Failed to perform IO control operation on ISO-TP socket {}: {}", interfaceName_, status.toString());
        return {STATUS::ERROR,
                ERROR::COMMUNICATION_ERROR,
                "FAILED to perform IO control operation on ISO-TP socket " + interfaceName_ +
                    "! Errors: " + std::strerror(errno),
                errno};
    }

    address.can_family = PF_CAN;
    address.can_ifindex = ifaceRequest.ifr_ifindex;
    address.can_addr.tp.rx_id = rxId_; // Receiver CAN ID
    address.can_addr.tp.tx_id = txId_; // Transmitter CAN ID

    auto addressPtr = std::make_shared<struct sockaddr_can>(address);
    status = socketOps_->bind(socketFd_, addressPtr, sizeof(address));
    if (status.isError()) {
        AERROR << "Failed to bind to ISO-TP socket: {}", status.toString());
        socketOps_->close(socketFd_);
        return {STATUS::ERROR,
                ERROR::ERROR_CREATING_DEVICE,
                "FAILED to bind to ISO-TP socket! Errors: " + std::string(std::strerror(errno)),
                errno};
    }

    ADEBUG <<
        "ISO-TP SocketCANDevice initialized on interface {} with RX ID: {}, TX ID: {}", interfaceName_, rxId_, txId_);
    return Status::OK();
}

Status SocketCANDevice::readSocket(std::vector<uint8_t>& buf) const
{
    Status status = socketOps_->read(socketFd_, buf, buf.size());
    if (status.isError()) {
        AERROR << "Failed to read from socket: {}", status.toString());
    } else {
        ADEBUG << "Successfully read from socket");
    }
    return status;
}

Status SocketCANDevice::writeToSocket(const std::vector<uint8_t>& buf, ssize_t& bytesWritten) const
{
    Status status = socketOps_->write(socketFd_, buf);
    if (status.isError()) {
        AERROR << "Failed to write to socket: {}", status.toString());
    } else {
        ADEBUG << "Successfully wrote to socket");
        bytesWritten = static_cast<ssize_t>(buf.size());
    }
    return status;
}

Status SocketCANDevice::closeSocket(int fd) const
{
    Status status = socketOps_->close(fd);
    if (status.isError()) {
        AERROR << "Failed to close socket: {}", status.toString());
    } else {
        ADEBUG << "Successfully closed socket");
    }
    return status;
}

Status SocketCANDevice::disableECU(int retry, double timeout)
{
    std::vector<uint8_t> diagRequest = {0x10, 0x03};             // Diagnostic session control request
    std::vector<uint8_t> comControlRequest = {0x28, 0x83, 0x01}; // Communication control disable request
    std::vector<uint8_t> response;                               // Buffer to receive responses
    response.reserve(8);

    for (int attempt = 0; attempt < retry; ++attempt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Send the diagnostic session control request
        ssize_t bytesWritten = 0;
        Status status = writeToSocket(diagRequest, bytesWritten);

        if (status.isError()) {
            AERROR <<
                "Send the diagnostic session control request.  Retry attempt {} out of {}", attempt + 1, retry);
            continue;
        }

        // Wait for a response
        status = readSocket(response);
        if (status.isError()) {
            AERROR << "Failed to read ISO-TP response: Retry attempt {} out of {}", attempt + 1, retry);
            continue;
        }

        // Check if the response is the expected diagnostic session control positive response
        if (response[0] == 0x50 && response[1] == 0x03) {
            ADEBUG << "EyeSight ECU Diagnostic session established.");

            bytesWritten = 0;
            status = writeToSocket(comControlRequest, bytesWritten);

            ADEBUG << "EyeSight ECU communication disabled.  Wrote {} bytes", bytesWritten);
            return Status::OK();
        }

        if (response[2] == 0x78) { // Handling a "response pending" code
            AERROR <<
                "Diagnostic control request pending, retrying....  Retry attempt {} out of {}", attempt + 1, retry);
        } else {
            AERROR << "Received response {} {} {}", response[0], response[1], response[2]);
            AERROR << "Unexpected response. Retrying... Retry attempt {} out of {}", attempt + 1, retry);
        }
    }

    std::cerr << "Failed to disable EyeSight ECU after " << retry << " attempts." << std::endl;
    return {STATUS::ERROR};
}

Status SocketCANDevice::close()
{
    ADEBUG << "Closing socket...");
    // TO-DO: check if it is open first;
    socketOps_->close(socketFd_);
    return Status::OK();
}

} // namespace platform::io::can
