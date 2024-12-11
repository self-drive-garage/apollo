// #include "platform/io/sockets/SocketOps.hpp"
// #include "platform/io/can/CANClient.hpp"
// #include "platform/io/can/SocketCANDevice.hpp"
// #include "platform/io/can/SocketCANMessage.hpp"
// #include "platform/io/can/dbc/CANDBC.hpp"
// #include "platform/io/can/dbc/SubaruGlobalCANDBC.hpp"
// #include "platform/transport/Transport.hpp"
// #include "platform/transport/TransportPublisher.hpp"
//
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/spdlog.h>
//
// #include <chrono>
// #include <fstream>
// #include <iostream>
// #include <memory>
// #include <mutex>
// #include <thread>
//
// using namespace solo::platform::transport;
// using namespace platform::io;
// using namespace platform::io::can;
// using namespace platform::io::can::dbc;
//
// std::mutex can_queue_mutex;
// int enqueue_period_ms = 100; // 10 Hz by default
//
// inline void store_serialized_data(std::ofstream& outfile,
//                                   dds::pub::DataWriter<CANDBCMessage>& writer,
//                                   const CANDBCMessage& data)
// {
//     // Access the serialized data
//     dds::core::InstanceHandle handle = writer.register_instance(data);
//     writer.write(data, handle);
//
//     // Access the serialized buffer
//     auto serialized_buffer = writer.get_serialized_data();
//
//     // Store serialized data directly
//     outfile.write(reinterpret_cast<const char*>(serialized_buffer.data()), serialized_buffer.size());
//     //
// }
//
// void record_can_data(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, std::ofstream& outfile)
// {
//
//     const auto publisher = Transport::CreatePublisher<CANDBCMessage>("CANDBCMessage");
//     auto writer = publisher.getDataWriter();
//
//     std::vector<CANDBCMessage> messages;
//
//     while (true) {
//         {
//             std::lock_guard<std::mutex> lock(can_queue_mutex);
//             messages = canClient->getQueuedMessagesAndClearQueue();
//         }
//         for (const auto& msg : messages) {
//             store_serialized_data(outfile, writer, msg);
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }
// }
//
// void enqueue_can_data(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
// {
//     while (true) {
//         auto start = std::chrono::steady_clock::now();
//         {
//             std::lock_guard<std::mutex> lock(can_queue_mutex);
//             canClient->enQueueMessages();
//         }
//         auto end = std::chrono::steady_clock::now();
//         std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
//     }
// }
//
// int main()
// {
//     auto socketOps = std::make_shared<SocketOps>();
//     auto canDevice = std::make_unique<SocketCANDevice>(socketOps);
//     auto canDatabase = CANDBC::CreateInstance();
//     auto canClient = std::make_unique<CANClient<SocketCANDevice>>(std::move(canDevice), std::move(canDatabase));
//
//     std::ofstream outfile("sensor_data.bin", std::ios::binary);
//
//     try {
//         std::thread enqueue_thread(enqueue_can_data, std::ref(canClient), enqueue_period_ms);
//         std::thread record_thread(record_can_data, std::ref(canClient), std::ref(outfile));
//
//         enqueue_thread.join();
//         record_thread.join();
//
//     } catch (const std::exception& e) {
//         AERROR << "Error recording CAN data: {}", e.what());
//         return EXIT_FAILURE;
//         outfile.close();
//     }
//
//     return EXIT_SUCCESS;
// }
