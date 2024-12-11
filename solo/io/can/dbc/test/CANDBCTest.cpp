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
#include "platform/io/can/dbc/CANDBC.hpp"

#include "platform/test/BaseTest.hpp"

#include <gtest/gtest.h>

using namespace platform::io::can::dbc;

class CANDBCTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override { dbc = CANDBC::CreateInstance(); }

    std::unique_ptr<CANDBC> dbc;
};

TEST_F(CANDBCTest, BufferSizeToDataLengthCode)
{
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(0), 0);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(8), 8);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(12), 9);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(16), 10);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(20), 11);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(24), 12);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(32), 13);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(48), 14);
    ASSERT_EQ(CANDBC::bufferSizeToDataLengthCode(64), 15);
}

TEST_F(CANDBCTest, GetMessageByAddress)
{
    auto message = dbc->getMessageByAddress(0x123);
    ASSERT_FALSE(message.has_value());
}

TEST_F(CANDBCTest, GetMessageByName)
{
    auto message = dbc->getMessageByName("testMessage");
    ASSERT_FALSE(message.has_value());
}

TEST_F(CANDBCTest, GetSignalSchemasByAddress)
{
    auto signals = dbc->getSignalSchemasByAddress(0x123);
    ASSERT_FALSE(signals.has_value());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
