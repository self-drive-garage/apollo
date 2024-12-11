#pragma once

#include "platform/io/sockets/ISocketOps.hpp"
#include "platform/io/Status.hpp"

#include <linux/can.h>
#include <linux/can/isotp.h>
#include <sys/socket.h>

#include <memory>
#include <vector>

namespace solo::platform::io {
/// @class SocketOps
/// @brief Implementation of ISocketOps for actual socket operations.
class SocketOps final : public ISocketOps
{
public:
    /// @brief Creates a socket.
    /// @param domain The communication domain.
    /// @param type The socket type.
    /// @param protocol The protocol to be used.
    /// @return A pair of Status object indicating success or error, and a file
    /// descriptor.  In case of an error the file descriptor is -1
    std::pair<Status, std::optional<int>> socket(int domain, int type, int protocol) override;

    /// @brief Manipulates file descriptor.
    /// @param fd The file descriptor to be manipulated.
    /// @param cmd The command to be performed.
    /// @param arg Optional argument for some commands.
    /// @return Status object indicating success or error.
    Status fcntl(int fd, int cmd, int64_t arg = 0) override;

    /// @brief Reads data from a file descriptor.
    /// @param fd The file descriptor to read from.
    /// @param buf A reference to the buffer where the read data will be stored.
    /// @param count The number of bytes to read.
    /// @return Status object indicating success or error.
    Status read(int fd, std::vector<uint8_t>& buf, size_t count) override;

    /// @brief Writes data to a file descriptor.
    /// @param fd The file descriptor to write to.
    /// @param buf A reference to the buffer containing the data to write.
    /// @return Status object indicating success or error.
    Status write(int fd, const std::vector<uint8_t>& buf) override;

    /// @brief Closes a file descriptor.
    /// @param fd The file descriptor to close.
    /// @return Status object indicating success or error.
    Status close(int fd) override;

    /// @brief Performs device-specific input/output operations.
    /// @tparam T The type of the argument passed to ioctl.
    /// @param fd The file descriptor of the device.
    /// @param request The request code.
    /// @param arg A reference to the argument used to exchange data between the caller and the device.
    /// @return Status object indicating success or error.
    template<typename T>
    Status ioctl(int fd, unsigned long request, T& arg)
    {
        return ioctl(fd, request, static_cast<void*>(&arg));
    }

    /// @brief Binds a name to a socket.
    /// @param fd The file descriptor of the socket.
    /// @param addr A shared pointer to a sockaddr structure containing the address to bind to.
    /// @param addrlen The length of the address structure.
    /// @return Status object indicating success or error.
    Status bind(int fd, std::shared_ptr<const sockaddr> addr, socklen_t addrlen) override;

    /// @brief Binds a name to a CAN socket.
    /// @param fd The file descriptor of the socket.
    /// @param addr A shared pointer to a sockaddr_can structure containing the address to bind to.
    /// @param addrlen The length of the address structure.
    /// @return Status object indicating success or error.
    Status bind(int fd, std::shared_ptr<const sockaddr_can> addr, socklen_t addrlen) override;

    /// @brief Sets the socket options for a CAN ISO-TP socket.
    /// @details This function sets various options for a CAN ISO-TP socket using the setsockopt system call.
    /// It allows configuring specific behavior and parameters of the socket to tailor its operation to the
    /// requirements of the application.
    ///
    /// @param fd The file descriptor of the socket.
    /// @param level The protocol level at which the option resides (typically SOL_CAN_ISOTP).
    /// @param optionName The specific option to set (e.g., CAN_ISOTP_OPTS).
    /// @param options A shared pointer to a can_isotp_options structure containing the desired settings.
    /// @param optionLength The size of the options structure.
    /// @return Status object indicating success or error. If the operation is successful, the status will indicate
    /// success;
    ///         otherwise, it will contain error details.
    Status setsockopt(int fd, int level, int optionName, std::shared_ptr<struct can_isotp_options> options) override;

protected:
    Status ioctl(int fd, unsigned long request, void* arg) override;
};
} // namespace platform::io
