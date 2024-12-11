#include "platform/io/can/CANClient.hpp"

#include "platform/io/Status.hpp"
#include "platform/io/can/CAN.hpp"
#include "platform/io/can/SocketCANDevice.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"
#include "platform/io/can/dbc/SubaruGlobalCANDBC.hpp"
#include "platform/io/mocks/MockSocketOps.hpp"
#include "platform/test/BaseTest.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/ioctl.h>

using namespace platform::io;
using namespace platform::io::can;
using namespace platform::io::can::dbc;
using namespace platform::io::mocks;
using namespace testing;

class CANClientTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override
    {
        //
        mockSocketOps = std::make_shared<NiceMock<MockSocketOps>>();
        auto canDBC = CANDBC::CreateInstance();

        EXPECT_CALL(*mockSocketOps, socket(_, _, _)).WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
        EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
        EXPECT_CALL(*mockSocketOps, fcntl(_, F_GETFL, 0)).WillOnce(Return(Status{STATUS::SUCCESS}));
        EXPECT_CALL(*mockSocketOps, fcntl(_, F_SETFL, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
        EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
            .WillOnce(Return(Status{STATUS::SUCCESS}));

        device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps);

        canClient = std::make_unique<CANClient<SocketCANDevice>>(std::move(device), std::move(canDBC));
    }

    std::shared_ptr<NiceMock<MockSocketOps>> mockSocketOps;
    std::unique_ptr<SocketCANDevice> device;
    std::unique_ptr<CANClient<SocketCANDevice>> canClient;
};

// TEST_F(CANClientTest, InitDeviceSuccess)
// {
//     EXPECT_CALL(*mockSocketOps, socket(_, _, _)).WillOnce(Return(std::make_pair(Status{STATUS::SUCCESS}, std::optional<int>(5))));
//     EXPECT_CALL(*mockSocketOps, ioctl(_, SIOCGIFINDEX, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, fcntl(_, F_GETFL, 0)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, fcntl(_, F_SETFL, _)).WillOnce(Return(Status{STATUS::SUCCESS}));
//     EXPECT_CALL(*mockSocketOps, bind(_, Matcher<std::shared_ptr<const sockaddr_can>>(_), _))
//         .WillOnce(Return(Status{STATUS::SUCCESS}));
//
//     EXPECT_NO_THROW(device = std::make_unique<SocketCANDevice>("vcan0", mockSocketOps));
// }

TEST_F(CANClientTest, EnQueueMessagesSuccess)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;
    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    EXPECT_CALL(*mockSocketOps, read(_, _, _))
        .WillOnce(DoAll(SetArgReferee<1>(buffer), Return(Status{STATUS::SUCCESS})));

    canClient->enQueueMessages();

    auto messages = canClient->getQueuedMessagesAndClearQueue();
    // ASSERT_FALSE(messages.empty());
}

TEST_F(CANClientTest, EnQueueMessagesFailure)
{
    EXPECT_CALL(*mockSocketOps, read(_, _, _)).WillOnce(Return(Status{STATUS::ERROR, ERROR::COMMUNICATION_ERROR}));

    canClient->enQueueMessages();

    auto messages = canClient->getQueuedMessagesAndClearQueue();
    ASSERT_TRUE(messages.empty());
}

TEST_F(CANClientTest, SendMessageSuccess)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;
    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    EXPECT_CALL(*mockSocketOps, write(_, _)).WillOnce(Return(Status{STATUS::SUCCESS}));

    CANDBCMessage message{frame.can_id, "Steering", CANBus::MAIN_BUS, {}};
    auto status = canClient->sendMessage<SocketCANMessage>(message);
    ASSERT_TRUE(status);
}

TEST_F(CANClientTest, SendMessageFailure)
{
    can_frame frame{};
    frame.can_id = 1;
    frame.can_dlc = 8;
    std::vector<uint8_t> buffer(sizeof(frame));
    std::memcpy(buffer.data(), &frame, sizeof(frame));

    // Indicate that the write method should not be called
    EXPECT_CALL(*mockSocketOps, write(_, _)).Times(0);

    CANDBCMessage message{frame.can_id, "Wrong Name", CANBus::MAIN_BUS, {}};
    auto status = canClient->sendMessage<SocketCANMessage>(message);
    ASSERT_FALSE(status);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
