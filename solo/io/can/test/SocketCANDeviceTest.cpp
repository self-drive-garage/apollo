#include "platform/io/can/SocketCANDevice.hpp"

#include "platform/io/Status.hpp"
#include "platform/io/can/SocketCANMessage.hpp"
#include "platform/io/can/exceptions/CANExceptions.hpp"
#include "platform/io/mocks/MockSocketOps.hpp"
#include "platform/test/BaseTest.hpp"

#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <linux/can.h>
#include <sys/ioctl.h>

using namespace platform::io::mocks;
using namespace platform::io::can;
using namespace platform::io;
using namespace testing;

class SocketCANDeviceTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override { mockSocketOps = std::make_shared<NiceMock<MockSocketOps>>(); }

    std::shared_ptr<NiceMock<MockSocketOps>> mockSocketOps;
    std::unique_ptr<SocketCANDevice> device;
};

TEST_F(SocketCANDeviceTest, InitDeviceSuccess)
{
    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
    EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
    EXPECT_CALL(*mockSocketOps, fcntl(_, F_GETFL, 0)).WillOnce(Return(Status{STATUS::SUCCESS}));
    EXPECT_CALL(*mockSocketOps, fcntl(_, F_SETFL, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
    EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
        .WillOnce(Return(Status{STATUS::SUCCESS}));

    EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));
}

TEST_F(SocketCANDeviceTest, InitDeviceFailure)
{
    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE}, std::nullopt)));

    EXPECT_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps), exceptions::CANInitException);
}
//
// TEST_F(SocketCANDeviceTest, InitIsoTpDeviceSuccess)
// {
//     EXPECT_CALL(*mockSocketOps, socket(_, _, _))
//         .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
//     EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
//         .WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, setsockopt(_, _, _, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//
//     EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps, true));
// }
//
// TEST_F(SocketCANDeviceTest, InitIsoTpDeviceSetSockOptsFailed)
// {
//     EXPECT_CALL(*mockSocketOps, socket(_, _, _))
//         .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
//     EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
//         .WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, setsockopt(_, _, _, _)).WillOnce(Return(Status{STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE}));
//
//     EXPECT_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps, true),
//                  exceptions::CANInitException);
// }
//
// TEST_F(SocketCANDeviceTest, InitIsoTpDeviceBindFailed)
// {
//     EXPECT_CALL(*mockSocketOps, socket(_, _, _))
//         .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
//     EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
//         .WillOnce(Return(Status{STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE}));
//
//     EXPECT_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps, true),
//                  exceptions::CANInitException);
// }
//
// TEST_F(SocketCANDeviceTest, InitIsoTpDeviceFailure)
// {
//     EXPECT_CALL(*mockSocketOps, socket(_, _, _))
//         .WillOnce(Return(std::make_pair(Status{STATUS::ERROR, ERROR::ERROR_CREATING_DEVICE}, std::nullopt)));
//
//     EXPECT_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps, true),
//                  exceptions::CANInitException);
// }

TEST_F(SocketCANDeviceTest, GetMessageSuccess)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;

    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(1))));
    EXPECT_CALL(*mockSocketOps, read(_, _, _))
        .WillOnce(DoAll(SetArgReferee<1>(buffer), Return(Status{STATUS::SUCCESS})));

    EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));
    auto [status, messageOpt] = device->getMessage();
    // ASSERT_TRUE(status.ok());
    // ASSERT_TRUE(messageOpt.has_value());
    // ASSERT_EQ(messageOpt->getRawFrame().can_id, 1);
}

TEST_F(SocketCANDeviceTest, GetMessageFailure)
{
    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(1))));
    EXPECT_CALL(*mockSocketOps, read(_, _, _)).WillOnce(Return(Status{STATUS::ERROR, ERROR::COMMUNICATION_ERROR}));

    EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));
    auto [status, messageOpt] = device->getMessage();
    ASSERT_TRUE(status.isError());
    ASSERT_FALSE(messageOpt.has_value());
}

TEST_F(SocketCANDeviceTest, SendMessageSuccess)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;
    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(1))));
    EXPECT_CALL(*mockSocketOps, write(_, _)).WillOnce(Return(Status{STATUS::SUCCESS}));

    EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));

    auto status = device->sendMessage(frame);
    ASSERT_TRUE(status.ok());
}

TEST_F(SocketCANDeviceTest, SendMessageFailure)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;
    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    EXPECT_CALL(*mockSocketOps, socket(_, _, _))
        .WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(1))));
    EXPECT_CALL(*mockSocketOps, write(_, _)).WillOnce(Return(Status{STATUS::ERROR, ERROR::COMMUNICATION_ERROR}));
    EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));

    auto status = device->sendMessage(frame);
    ASSERT_TRUE(status.isError());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
