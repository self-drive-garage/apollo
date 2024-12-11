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

/// @file CANDBCMessageTest.cpp
/// @brief This file contains the unit tests for the CANDBCSignal and CANDBCMessage classes.
///
#include "platform/io/can/dbc/CANDBCMessage.hpp"

#include "platform/io/can/CAN.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"
#include "platform/test/BaseTest.hpp"

#include <gtest/gtest.h>

using namespace platform::io::can;
using namespace platform::io::can::dbc;

class CANDBCSignalTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override { signal = new CANDBCSignal(0, "testSignal", 1.23, true); }

    void TearDown() override { delete signal; }

    CANDBCSignal* signal;
};

TEST_F(CANDBCSignalTest, GetName)
{
    ASSERT_EQ(signal->name(), "testSignal");
}

TEST_F(CANDBCSignalTest, GetValue)
{
    ASSERT_DOUBLE_EQ(signal->value(), 1.23);
}

TEST_F(CANDBCSignalTest, GetTimestampNanoSeconds)
{
    signal->timestampNanoSeconds(1000000000);
    ASSERT_EQ(signal->timestampNanoSeconds(), 1000000000);
}

TEST_F(CANDBCSignalTest, SetName)
{
    signal->name("newName");
    ASSERT_EQ(signal->name(), "newName");
}

TEST_F(CANDBCSignalTest, SetValue)
{
    signal->value(4.56);
    ASSERT_DOUBLE_EQ(signal->value(), 4.56);
}

TEST_F(CANDBCSignalTest, SetTimestampNanoSeconds)
{
    signal->timestampNanoSeconds(2000000000);
    ASSERT_EQ(signal->timestampNanoSeconds(), 2000000000);
}

class CANDBCMessageTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        CANDBCSignal signal1(0, "signal1", 1.23, true);
        CANDBCSignal signal2(0, "signal2", 4.56, true);
        signals.push_back(signal1);
        signals.push_back(signal2);
        message = new CANDBCMessage(0x123, "testMessage", CANBus::MAIN_BUS, signals);
    }

    void TearDown() override { delete message; }

    std::vector<CANDBCSignal> signals;
    CANDBCMessage* message;
};

TEST_F(CANDBCMessageTest, GetAddress)
{
    ASSERT_EQ(message->address(), 0x123);
}

TEST_F(CANDBCMessageTest, GetName)
{
    ASSERT_EQ(message->name(), "testMessage");
}

TEST_F(CANDBCMessageTest, GetCANBus)
{
    ASSERT_EQ(message->getCanBus(), "MAIN_BUS");
}

TEST_F(CANDBCMessageTest, GetSignals)
{
    auto sigs = message->signals();
    ASSERT_EQ(sigs.size(), 2);
    ASSERT_EQ(sigs[0].name(), "signal1");
    ASSERT_EQ(sigs[1].name(), "signal2");
}

TEST_F(CANDBCMessageTest, SetAddress)
{
    message->address(0x456);
    ASSERT_EQ(message->address(), 0x456);
}

TEST_F(CANDBCMessageTest, SetName)
{
    message->name("newMessage");
    ASSERT_EQ(message->name(), "newMessage");
}

TEST_F(CANDBCMessageTest, SetCANBus)
{
    message->canBus(CANBus::MAIN_BUS);
    ASSERT_EQ(message->getCanBus(), "MAIN_BUS");
}

TEST_F(CANDBCMessageTest, SetSignals)
{
    std::vector<CANDBCSignal> newSignals;
    CANDBCSignal signal1(0, "newSignal1", 7.89, true);
    CANDBCSignal signal2(0, "newSignal2", 0.12, true);
    newSignals.push_back(signal1);
    newSignals.push_back(signal2);

    message->signals(newSignals);
    auto sigs = message->signals();
    ASSERT_EQ(sigs.size(), 2);
    ASSERT_EQ(sigs[0].name(), "newSignal1");
    ASSERT_EQ(sigs[1].name(), "newSignal2");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
