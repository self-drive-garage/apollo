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
#include "platform/io/can/dbc/CANDBCMessageSchema.hpp"

#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"
#include "platform/test/BaseTest.hpp"

#include <gtest/gtest.h>

using namespace platform::io::can::dbc;

class CANDBCMessageSchemaTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override
    {
        CANDBCSignalSchema::ValueDescription valueDesc = {"testValue", 0x123, "default"};
        CANDBCSignalSchema signal(
            "testSignal", "testMessage", 0, 7, 0, 8, true, 1.0, 0.0, true, SignalType::DEFAULT, valueDesc);

        signals.push_back(signal);
        signalNameToSignalMap.emplace("testSignal", signal);

        schema = new CANDBCMessageSchema("testMessage", 0x123, 8, signals, signalNameToSignalMap);
    }

    void TearDown() override { delete schema; }

    std::vector<CANDBCSignalSchema> signals;
    std::unordered_map<std::string, const CANDBCSignalSchema> signalNameToSignalMap;
    CANDBCMessageSchema* schema;
};

TEST_F(CANDBCMessageSchemaTest, GetName)
{
    ASSERT_EQ(schema->getName(), "testMessage");
}

TEST_F(CANDBCMessageSchemaTest, SetName)
{
    schema->setName("newMessageName");
    ASSERT_EQ(schema->getName(), "newMessageName");
}

TEST_F(CANDBCMessageSchemaTest, GetAddress)
{
    ASSERT_EQ(schema->getAddress(), 0x123);
}

TEST_F(CANDBCMessageSchemaTest, SetAddress)
{
    schema->setAddress(0x456);
    ASSERT_EQ(schema->getAddress(), 0x456);
}

TEST_F(CANDBCMessageSchemaTest, GetSize)
{
    ASSERT_EQ(schema->getSize(), 8);
}

TEST_F(CANDBCMessageSchemaTest, SetSize)
{
    schema->setSize(16);
    ASSERT_EQ(schema->getSize(), 16);
}

TEST_F(CANDBCMessageSchemaTest, GetSignalSchemaByName)
{
    auto signal = schema->getSignalSchemaByName("testSignal");
    ASSERT_TRUE(signal.has_value());
    ASSERT_EQ(signal->get().getName(), "testSignal");
}

TEST_F(CANDBCMessageSchemaTest, MoveSignalSchemaToMap)
{
    CANDBCSignalSchema::ValueDescription valueDesc = {"newValue", 0x456, "newDefault"};
    CANDBCSignalSchema newSignal(
        "newSignal", "newMessage", 8, 15, 8, 8, false, 2.0, 1.0, false, SignalType::COUNTER, valueDesc);

    schema->moveSignalSchemaToMap("newSignal", newSignal);
    auto signal = schema->getSignalSchemaByName("newSignal");
    ASSERT_TRUE(signal.has_value());
    ASSERT_EQ(signal->get().getName(), "newSignal");
}

TEST_F(CANDBCMessageSchemaTest, GetSignalNameToSignalMap)
{
    auto map = schema->getSignalNameToSignalMap();
    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.at("testSignal").getName(), "testSignal");
}

TEST_F(CANDBCMessageSchemaTest, SetSignalNameToSignalMap)
{
    std::unordered_map<std::string, const CANDBCSignalSchema> newMap;
    CANDBCSignalSchema::ValueDescription valueDesc = {"newValue", 0x456, "newDefault"};
    CANDBCSignalSchema newSignal(
        "newSignal", "newMessage", 8, 15, 8, 8, false, 2.0, 1.0, false, SignalType::COUNTER, valueDesc);
    newMap.emplace("newSignal", newSignal);

    schema->setSignalNameToSignalMap(newMap);
    auto map = schema->getSignalNameToSignalMap();
    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.at("newSignal").getName(), "newSignal");
}

TEST_F(CANDBCMessageSchemaTest, GetSignals)
{
    auto sigs = schema->getSignals();
    ASSERT_EQ(sigs.size(), 1);
    ASSERT_EQ(sigs[0].getName(), "testSignal");
}

TEST_F(CANDBCMessageSchemaTest, SetSignals)
{
    std::vector<CANDBCSignalSchema> newSignals;
    CANDBCSignalSchema::ValueDescription valueDesc = {"newValue", 0x456, "newDefault"};
    CANDBCSignalSchema newSignal(
        "newSignal", "newMessage", 8, 15, 8, 8, false, 2.0, 1.0, false, SignalType::COUNTER, valueDesc);
    newSignals.push_back(newSignal);

    schema->setSignals(newSignals);
    auto sigs = schema->getSignals();
    ASSERT_EQ(sigs.size(), 1);
    ASSERT_EQ(sigs[0].getName(), "newSignal");
}

TEST_F(CANDBCMessageSchemaTest, CreateCANDBCMessage)
{

    std::unordered_map<std::string, double> signalValueMap{
        {"testSignal", 1.0}
    };

    auto msg = schema->createCANDBCMessage(signalValueMap);

    ASSERT_EQ(msg.name(), schema->getName());
    ASSERT_EQ(msg.address(), schema->getAddress());
    ASSERT_EQ(msg.signals().size(), schema->getSignals().size());
    ASSERT_EQ(msg.signals()[0].name(), "testSignal");
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
