// #include "platform/sensors/gps/GPSSensor.hpp"

// #include "platform/io/Status.hpp"
// #include "platform/sensors/mocks/MockGPSReceiver.hpp"

// #include <gmock/gmock.h>
// #include <gtest/gtest.h>

// using namespace platform::sensors;
// using namespace platform::sensors::mocks;
// using namespace testing;

// class GPSSensorTest : public ::testing::Test
// {
// protected:
//     void SetUp() override {}
// };

// TEST_F(GPSSensorTest, OpenSuccess)
// {
//     std::unique_ptr<MockGPSReceiver> mockGPSReceiver;
//     mockGPSReceiver = std::make_unique<MockGPSReceiver>();
    
//     EXPECT_CALL(*mockGPSReceiver, open()).WillOnce(Return(platform::io::Status{platform::io::STATUS::SUCCESS}));

//     std::unique_ptr<GPSSensor<MockGPSReceiver>> gpsSensor;
//     gpsSensor = std::make_unique<GPSSensor<MockGPSReceiver>>(std::move(mockGPSReceiver));

//     auto status = gpsSensor->open();
//     EXPECT_TRUE(status.ok());
//     EXPECT_TRUE(gpsSensor->isOpened());
// }

// TEST_F(GPSSensorTest, OpenFailure) {
//     std::unique_ptr<MockGPSReceiver> mockGPSReceiver;
//     mockGPSReceiver = std::make_unique<MockGPSReceiver>();

//     EXPECT_CALL(*mockGPSReceiver, open())
//         .WillOnce(Return(platform::io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::ERROR_OPENING_DEVICE}));

//     std::unique_ptr<GPSSensor<MockGPSReceiver>> gpsSensor;
//     gpsSensor = std::make_unique<GPSSensor<MockGPSReceiver>>(std::move(mockGPSReceiver));

//     auto status = gpsSensor->open();
    
//     EXPECT_FALSE(status.ok());
//     EXPECT_FALSE(gpsSensor->isOpened());
// }

// TEST_F(GPSSensorTest, CloseSuccess) {
//     std::unique_ptr<MockGPSReceiver> mockGPSReceiver;
//     mockGPSReceiver = std::make_unique<MockGPSReceiver>();


//     EXPECT_CALL(*mockGPSReceiver, close())
//         .WillOnce(Return(platform::io::Status{platform::io::STATUS::SUCCESS}));

//     std::unique_ptr<GPSSensor<MockGPSReceiver>> gpsSensor;
//     gpsSensor = std::make_unique<GPSSensor<MockGPSReceiver>>(std::move(mockGPSReceiver));

//     gpsSensor->open(); // Assume open is successful
//     auto status = gpsSensor->close();
//     EXPECT_TRUE(status.ok());
//     EXPECT_FALSE(gpsSensor->isOpened());
// }

// TEST_F(GPSSensorTest, ReadSuccess) {

//     GPSData data;

//     std::unique_ptr<MockGPSReceiver> mockGPSReceiver;
//     mockGPSReceiver = std::make_unique<MockGPSReceiver>();
    
//     EXPECT_CALL(*mockGPSReceiver, open()).WillOnce(Return(platform::io::Status{platform::io::STATUS::SUCCESS}));
//     EXPECT_CALL(*mockGPSReceiver, read(_))
//         .WillOnce(Return(std::make_tuple(platform::io::Status{platform::io::STATUS::SUCCESS}, std::make_optional(data))));


//     std::unique_ptr<GPSSensor<MockGPSReceiver>> gpsSensor;
//     gpsSensor = std::make_unique<GPSSensor<MockGPSReceiver>>(std::move(mockGPSReceiver));


//     gpsSensor->open();
//     auto [status, readData] = gpsSensor->read();
//     EXPECT_TRUE(status.ok());
//     EXPECT_TRUE(readData.has_value());
// }

// TEST_F(GPSSensorTest, ReadFailure) {

//     GPSData data;

//     std::unique_ptr<MockGPSReceiver> mockGPSReceiver;
//     mockGPSReceiver = std::make_unique<MockGPSReceiver>();
    
//     EXPECT_CALL(*mockGPSReceiver, open()).WillOnce(Return(platform::io::Status{platform::io::STATUS::SUCCESS}));
//     EXPECT_CALL(*mockGPSReceiver, read(_))
//         .WillOnce(Return(std::make_tuple(platform::io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::COMMUNICATION_ERROR},
//         std::nullopt)));


//     std::unique_ptr<GPSSensor<MockGPSReceiver>> gpsSensor;
//     gpsSensor = std::make_unique<GPSSensor<MockGPSReceiver>>(std::move(mockGPSReceiver));

//     gpsSensor->open();
//     auto [status, readData] = gpsSensor->read();
    
//     EXPECT_TRUE(status.isError());
//     EXPECT_FALSE(status.ok());
//     EXPECT_FALSE(readData.has_value());

// }

// int main(int argc, char** argv)
// {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
