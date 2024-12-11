#include "platform/io/sockets/SocketOps.hpp"

#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/isotp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace solo::platform::io {

std::pair<Status, std::optional<int>> SocketOps::socket(const int domain, const int type, const int protocol)
{
    int fd = std::numeric_limits<int>::min();
    if (fd = ::socket(domain, type, protocol); fd == -1) {
        return {{STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, std::strerror(errno), errno}, std::nullopt};
    }
    return {{STATUS::SUCCESS}, fd};
}

Status SocketOps::ioctl(const int fd, unsigned long request, void* arg)
{
    if (const int result = ::ioctl(fd, request, arg); result == -1) {
        return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, std::strerror(errno), errno};
    }
    return Status::OK();
}

Status SocketOps::fcntl(const int fd, const int cmd, const int64_t arg)
{
    if (const int result = ::fcntl(fd, cmd, arg); result == -1) {
        return {STATUS::ERROR, ERROR::INVALID_PARAMETER, std::strerror(errno), errno};
    }
    return Status::OK();
}

Status SocketOps::bind(const int fd, std::shared_ptr<const struct sockaddr> addr, const socklen_t addrlen)
{
    if (const int result = ::bind(fd, addr.get(), addrlen); result == -1) {
        return {STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, std::strerror(errno), errno};
    }
    return Status::OK();
}

Status SocketOps::bind(const int fd, std::shared_ptr<const struct sockaddr_can> addr, const socklen_t addrlen)
{
    if (const int result = ::bind(fd, reinterpret_cast<const struct sockaddr*>(addr.get()), addrlen); result == -1) {
        return {STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE, std::strerror(errno), errno};
    }
    return Status::OK();
}

Status SocketOps::setsockopt(int fd, int level, int optionName, std::shared_ptr<struct can_isotp_options> options)
{
    socklen_t n = sizeof(can_isotp_options);
    // memset(options.get(), 0, n);
    options->flags |= CAN_ISOTP_FORCE_TXSTMIN; // Optional: force transmission spacing
    if (const int result = ::setsockopt(fd, level, optionName, options.get(), n); result < 0) {
        return {STATUS::ERROR, ERROR::ERROR_SETTING_SOCKET_ISO_TP_OPTIONS, std::strerror(errno), errno};
    }

    return Status::OK();
}

Status SocketOps::read(const int fd, std::vector<uint8_t>& buf, const size_t count)
{
    ssize_t bytesRead = 0;
    buf.resize(count);
    if (bytesRead = ::read(fd, buf.data(), count); bytesRead == -1) {
        return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, std::strerror(errno), errno};
    }
    buf.resize(bytesRead); // resize to actual bytes read
    return Status::OK();
}

Status SocketOps::write(const int fd, const std::vector<uint8_t>& buf)
{
    // if (const ssize_t bytesWritten = ::write(fd, buf.data(), buf.size()); bytesWritten == -1) {
    //     return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, std::strerror(errno), errno};
    // }
    // return Status::OK();

    ssize_t bytesWritten = ::write(fd, buf.data(), buf.size());
    if (bytesWritten == -1) {
        // AERROR << "Write error: {}", std::strerror(errno));
        return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, std::strerror(errno), errno};
    }
    // ADEBUG << "Bytes written: {}", bytesWritten);
    return Status::OK();

}

Status SocketOps::close(const int fd)
{
    if (const int result = ::close(fd); result == -1) {
        return {STATUS::ERROR, ERROR::COMMUNICATION_ERROR, std::strerror(errno), errno};
    }
    return Status::OK();
}

} // namespace platform::io
