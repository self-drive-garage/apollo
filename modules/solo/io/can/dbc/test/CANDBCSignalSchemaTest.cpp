/// @file CANDBCSignalSchemaTest.cpp
/// @brief This file contains the unit tests for the CANDBCSignalSchema class.
///
#include "platform/io/can/dbc/CANDBCMessage.hpp"
#include "platform/io/can/dbc/CANDBCSignalSchema.hpp"

#include "platform/test/BaseTest.hpp"

#include <gtest/gtest.h>

using namespace platform::io::can::dbc;

class CANDBCSignalSchemaTest : public ::platform::tests::BaseTest
{
protected:
    void SetUp() override
    {
        CANDBCSignalSchema::ValueDescription valueDesc = {"testValue", 0x123, "default"};
        schema = new CANDBCSignalSchema(
            "testSignal", "testMessage", 0, 7, 0, 8, true, 1.0, 0.0, true, SignalType::DEFAULT, valueDesc);
    }

    void TearDown() override { delete schema; }

    CANDBCSignalSchema* schema;
};

TEST_F(CANDBCSignalSchemaTest, GetName)
{
    ASSERT_EQ(schema->getName(), "testSignal");
}

TEST_F(CANDBCSignalSchemaTest, GetMessageName)
{
    ASSERT_EQ(schema->getMessageName(), "testMessage");
}

TEST_F(CANDBCSignalSchemaTest, GetStartBit)
{
    ASSERT_EQ(schema->getStartBit(), 0);
}

TEST_F(CANDBCSignalSchemaTest, GetMostSignificantBit)
{
    ASSERT_EQ(schema->getMostSignificantBit(), 7);
}

TEST_F(CANDBCSignalSchemaTest, GetLeastSignificantBit)
{
    ASSERT_EQ(schema->getLeastSignificantBit(), 0);
}

TEST_F(CANDBCSignalSchemaTest, GetSize)
{
    ASSERT_EQ(schema->getSize(), 8);
}

TEST_F(CANDBCSignalSchemaTest, IsSigned)
{
    ASSERT_TRUE(schema->isSigned());
}

TEST_F(CANDBCSignalSchemaTest, GetFactor)
{
    ASSERT_DOUBLE_EQ(schema->getFactor(), 1.0);
}

TEST_F(CANDBCSignalSchemaTest, GetOffset)
{
    ASSERT_DOUBLE_EQ(schema->getOffset(), 0.0);
}

TEST_F(CANDBCSignalSchemaTest, IsLittleEndian)
{
    ASSERT_TRUE(schema->isLittleEndian());
}

TEST_F(CANDBCSignalSchemaTest, GetSignalType)
{
    ASSERT_EQ(schema->getSignalType(), SignalType::DEFAULT);
}

TEST_F(CANDBCSignalSchemaTest, GetValueDescription)
{
    auto valueDesc = schema->getValueDescription();
    ASSERT_EQ(valueDesc.name, "testValue");
    ASSERT_EQ(valueDesc.address, 0x123);
    ASSERT_EQ(valueDesc.def_val, "default");
}

TEST_F(CANDBCSignalSchemaTest, ParseValue)
{
    std::vector<uint8_t> messageData = {0x01};
    ASSERT_EQ(schema->parseValue(messageData), 1);
}

// TEST_F(CANDBCSignalSchemaTest, CalcSubaruChecksum)
// {
//     std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
//     uint8_t checksum = schema->calcSubaruChecksum(0x123, data);
//     ASSERT_EQ(checksum, 0x00); // Placeholder for actual checksum logic
// }

TEST_F(CANDBCSignalSchemaTest, SetName)
{
    schema->setName("newName");
    ASSERT_EQ(schema->getName(), "newName");
}

TEST_F(CANDBCSignalSchemaTest, SetMessageName)
{
    schema->setMessageName("newMessageName");
    ASSERT_EQ(schema->getMessageName(), "newMessageName");
}

TEST_F(CANDBCSignalSchemaTest, SetStartBit)
{
    schema->setStartBit(1);
    ASSERT_EQ(schema->getStartBit(), 1);
}

TEST_F(CANDBCSignalSchemaTest, SetMostSignificantBit)
{
    schema->setMostSignificantBit(6);
    ASSERT_EQ(schema->getMostSignificantBit(), 6);
}

TEST_F(CANDBCSignalSchemaTest, SetLeastSignificantBit)
{
    schema->setLeastSignificantBit(1);
    ASSERT_EQ(schema->getLeastSignificantBit(), 1);
}

TEST_F(CANDBCSignalSchemaTest, SetSize)
{
    schema->setSize(16);
    ASSERT_EQ(schema->getSize(), 16);
}

TEST_F(CANDBCSignalSchemaTest, SetIsSigned)
{
    schema->setIsSigned(false);
    ASSERT_FALSE(schema->isSigned());
}

TEST_F(CANDBCSignalSchemaTest, SetFactor)
{
    schema->setFactor(2.0);
    ASSERT_DOUBLE_EQ(schema->getFactor(), 2.0);
}

TEST_F(CANDBCSignalSchemaTest, SetOffset)
{
    schema->setOffset(1.0);
    ASSERT_DOUBLE_EQ(schema->getOffset(), 1.0);
}

TEST_F(CANDBCSignalSchemaTest, SetIsLittleEndian)
{
    schema->setIsLittleEndian(false);
    ASSERT_FALSE(schema->isLittleEndian());
}

TEST_F(CANDBCSignalSchemaTest, SetType)
{
    schema->setType(SignalType::CHECKSUM);
    ASSERT_EQ(schema->getType(), SignalType::CHECKSUM);
}

TEST_F(CANDBCSignalSchemaTest, SetValueDescription)
{
    CANDBCSignalSchema::ValueDescription newValueDesc = {"newValue", 0x456, "newDefault"};
    schema->setValueDescription(newValueDesc);
    auto valueDesc = schema->getValueDescription();
    ASSERT_EQ(valueDesc.name, "newValue");
    ASSERT_EQ(valueDesc.address, 0x456);
    ASSERT_EQ(valueDesc.def_val, "newDefault");
}

TEST_F(CANDBCSignalSchemaTest, PackValue)
{
    std::vector<uint8_t> writeBuffer(8, 0);
    int64_t value = 1;
    schema->packValue(writeBuffer, value);
    ASSERT_EQ(writeBuffer[0], 1);
}

TEST_F(CANDBCSignalSchemaTest, PackValueDifferentStartBit)
{
    CANDBCSignalSchema schemaDifferentStartBit("testSignal",
                                               "testMessage",
                                               3,
                                               10,
                                               3,
                                               8,
                                               true,
                                               1.0,
                                               0.0,
                                               true,
                                               SignalType::DEFAULT,
                                               {"testValue", 0x123, "default"});
    std::vector<uint8_t> writeBuffer(8, 0);
    int64_t value = 1;
    schemaDifferentStartBit.packValue(writeBuffer, value);
    ASSERT_EQ(writeBuffer[0], 8); // 1 shifted left by 3 bits (8 = 1 << 3)
}

TEST_F(CANDBCSignalSchemaTest, PackValueLargerValue)
{
    std::vector<uint8_t> writeBuffer(8, 0);
    int64_t value = 255;
    schema->packValue(writeBuffer, value);
    ASSERT_EQ(writeBuffer[0], 255); // 255 fits in 8 bits
}

TEST_F(CANDBCSignalSchemaTest, PackValueLittleEndian)
{
    CANDBCSignalSchema schemaLittleEndian("testSignal",
                                          "testMessage",
                                          0,
                                          15,
                                          0,
                                          16,
                                          true,
                                          1.0,
                                          0.0,
                                          true,
                                          SignalType::DEFAULT,
                                          {"testValue", 0x123, "default"});
    std::vector<uint8_t> writeBuffer(8, 0);
    int64_t value = 0xABCD;
    schemaLittleEndian.packValue(writeBuffer, value);
    ASSERT_EQ(writeBuffer[0], 0xCD);
    ASSERT_EQ(writeBuffer[1], 0xAB);
}

TEST_F(CANDBCSignalSchemaTest, PackValueBigEndian)
{
    CANDBCSignalSchema schemaBigEndian("testSignal",
                                       "testMessage",
                                       0,
                                       15,
                                       0,
                                       16,
                                       true,
                                       1.0,
                                       0.0,
                                       false,
                                       SignalType::DEFAULT,
                                       {"testValue", 0x123, "default"});
    std::vector<uint8_t> writeBuffer(8, 0);
    int64_t value = 0xABCD;
    schemaBigEndian.packValue(writeBuffer, value);
    ASSERT_EQ(writeBuffer[0], 0xCD); // Least significant byte
}

TEST_F(CANDBCSignalSchemaTest, CreateCANDBCMessage)
{
    CANDBCSignalSchema schemaLittleEndian("testSignal",
                                          "testMessage",
                                          0,
                                          15,
                                          0,
                                          16,
                                          true,
                                          1.0,
                                          0.0,
                                          true,
                                          SignalType::DEFAULT,
                                          {"testValue", 0x123, "default"});

    auto signal = schemaLittleEndian.createCANDBCSignal(1.0);
    ASSERT_EQ(signal.name(), schemaLittleEndian.getName());
    ASSERT_EQ(signal.value(), 1.0);
    ASSERT_EQ(signal.isLittleEndian(), schemaLittleEndian.isLittleEndian());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
