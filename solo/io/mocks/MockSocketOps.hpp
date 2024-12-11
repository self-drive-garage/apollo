#pragma once

#include "platform/io/sockets/ISocketOps.hpp"
#include "platform/io/Status.hpp"

#include <gmock/gmock.h>
#include <linux/can.h>
#include <linux/can/isotp.h>
#include <sys/socket.h>

#include <optional>
#include <utility>

namespace platform::io::mocks {
class MockSocketOps : public ISocketOps
{
public:
    MOCK_METHOD((std::pair<Status, std::optional<int>>), socket, (int domain, int type, int protocol), (override));
    MOCK_METHOD(Status, fcntl, (int fd, int cmd, int64_t arg), (override));
    MOCK_METHOD(Status,
                setsockopt,
                (int fd, int level, int optionName, std::shared_ptr<struct can_isotp_options> options),
                (override));
    MOCK_METHOD(Status, read, (int fd, std::vector<uint8_t>& buf, size_t count), (override));
    MOCK_METHOD(Status, write, (int fd, const std::vector<uint8_t>& buf), (override));
    MOCK_METHOD(Status, close, (int fd), (override));
    MOCK_METHOD(Status, ioctl, (int fd, unsigned long request, void* arg), (override));
    MOCK_METHOD(Status, bind, (int fd, std::shared_ptr<const struct sockaddr>, socklen_t), (override));
    MOCK_METHOD(Status, bind, (int fd, std::shared_ptr<const struct sockaddr_can>, socklen_t), (override));
};
} // namespace platform::io::mocks
