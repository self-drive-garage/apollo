#include "platform/io/can/SocketCANMessage.hpp"

#include "platform/io/can/CAN.hpp"
#include "platform/io/can/dbc/CANDBCMessage.hpp"
#include "platform/test/BaseTest.hpp"

#include <gtest/gtest.h>

using namespace platform::io::can;
using namespace platform::io::can::dbc;

class SocketCANMessageTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override
    {
        can_frame frame;
        frame.can_id = 0x123;
        frame.can_dlc = 8;
        std::fill(std::begin(frame.data), std::end(frame.data), 0xFF);

        socketCANMessageFromFrame = std::make_unique<SocketCANMessage>(frame);
        socketCANMessageFromData = std::make_unique<SocketCANMessage>(0x123, std::vector<uint8_t>(8, 0xFF));
    }

    std::unique_ptr<SocketCANMessage> socketCANMessageFromFrame;
    std::unique_ptr<SocketCANMessage> socketCANMessageFromData;
};

TEST_F(SocketCANMessageTest, GetAddress)
{
    ASSERT_EQ(socketCANMessageFromFrame->getAddress(), 0x123);
    ASSERT_EQ(socketCANMessageFromData->getAddress(), 0x123);
}

TEST_F(SocketCANMessageTest, IsValid)
{
    ASSERT_TRUE(socketCANMessageFromFrame->isValid());
    ASSERT_TRUE(socketCANMessageFromData->isValid());
}

TEST_F(SocketCANMessageTest, GetCanId)
{
    ASSERT_EQ(socketCANMessageFromFrame->getCanId(), 0x123);
    ASSERT_EQ(socketCANMessageFromData->getCanId(), 0x123);
}

TEST_F(SocketCANMessageTest, GetFrameData)
{
    const std::vector<uint8_t>& frameData = socketCANMessageFromFrame->getFrameData();
    ASSERT_EQ(frameData.size(), 8);
    for (auto byte : frameData) {
        ASSERT_EQ(byte, 0xFF);
    }

    const std::vector<uint8_t>& frameDataFromData = socketCANMessageFromData->getFrameData();
    ASSERT_EQ(frameDataFromData.size(), 8);
    for (auto byte : frameDataFromData) {
        ASSERT_EQ(byte, 0xFF);
    }
}

TEST_F(SocketCANMessageTest, GetRawFrame)
{
    can_frame rawFrame = socketCANMessageFromFrame->getRawFrame();
    ASSERT_EQ(rawFrame.can_id, 0x123);
    ASSERT_EQ(rawFrame.can_dlc, 8);
    for (auto byte : rawFrame.data) {
        ASSERT_EQ(byte, 0xFF);
    }

    can_frame rawFrameFromData = socketCANMessageFromData->getRawFrame();
    ASSERT_EQ(rawFrameFromData.can_id, 0x123);
    ASSERT_EQ(rawFrameFromData.can_dlc, 8);
    for (auto byte : rawFrameFromData.data) {
        ASSERT_EQ(byte, 0xFF);
    }
}

TEST_F(SocketCANMessageTest, FromCANDBCMessage)
{
    CANDBCMessage canDBCMessage(0x123, "testMessage", CANBus::MAIN_BUS, {});
    std::vector<uint8_t> frameData(8, 0xFF);

    SocketCANMessage message = SocketCANMessage::fromCANDBCMessage(canDBCMessage, frameData);

    ASSERT_EQ(message.getAddress(), 0x123);
    ASSERT_TRUE(message.isValid());
    ASSERT_EQ(message.getCanId(), 0x123);

    const std::vector<uint8_t>& data = message.getFrameData();
    ASSERT_EQ(data.size(), 8);
    for (auto byte : data) {
        ASSERT_EQ(byte, 0xFF);
    }

    can_frame rawFrame = message.getRawFrame();
    ASSERT_EQ(rawFrame.can_id, 0x123);
    ASSERT_EQ(rawFrame.can_dlc, 8);
    for (auto byte : rawFrame.data) {
        ASSERT_EQ(byte, 0xFF);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
