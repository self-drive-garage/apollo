#include "platform/io/sockets/SocketOps.hpp"
#include "platform/io/can/CANClient.hpp"
#include "platform/io/can/SocketCANDevice.hpp"
#include "platform/io/can/SocketCANMessage.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"

#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace platform::io;
using namespace platform::io::can;
using namespace platform::io::can::dbc;

std::mutex can_queue_mutex;
std::atomic<bool> running(true);
int enqueue_period_ms = 20; // 50 Hz by default
uint32_t GLOBAL_ES_ADDR = 0x787;
uint64_t frameCounter_;

void handle_signal(int signal)
{
    if (signal == SIGINT) {
        running.store(false);
    }
}

void printHexVector(const std::vector<uint8_t>& vec)
{
    for (const auto& byte : vec) {
        // Print each byte as a two-digit hexadecimal value
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal and end the line
}

void send_disable_eyesight_ecu_can_message(std::unique_ptr<CANClient<SocketCANDevice>>& canClient)
{
    std::vector<uint8_t> diagRequest = {
        0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Diagnostic session control request
    std::vector<uint8_t> comControlRequest = {
        0x28, 0x83, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}; // Communication control disable request
    std::vector<uint8_t> response;                       // Buffer to receive responses

    assert(diagRequest.size() == 8);
    assert(comControlRequest.size() == 8);

    AINFO << "Sending the request EyeSight ECU to enter diagnostics mode");

    // send the diagnostic request and listen for a response on the same address
    auto resp = canClient->sendRawMessage<SocketCANMessage>(GLOBAL_ES_ADDR, diagRequest);

    if (auto [status, result] =
            canClient->listenForMessageByAddress<SocketCANMessage>(GLOBAL_ES_ADDR, std::chrono::milliseconds(500));
        status.isError()) {

        // let's try again by sending the same diagnostics request message
        // but listen on the standard ISO-TP address (address + 8)
        resp = canClient->sendRawMessage<SocketCANMessage>(GLOBAL_ES_ADDR, diagRequest);

        if (auto [status1, result1] = canClient->listenForMessageByAddress<SocketCANMessage>(
                GLOBAL_ES_ADDR + 0x08, std::chrono::milliseconds(500));
            status1.isError()) {
            AINFO << "Unable to read a response, let's proceed anyways");
        } else {
            AINFO << "We received data on the GLOBAL_ES_ADDR + 0x08.  Logging the data");
            if (result1.has_value()) {
                SocketCANMessage msg = result1.value();
                printHexVector(msg.getFrameData());
            }
        }
    } else {
        AINFO << "We received data on the original GLOBAL_ES_ADDR.  Logging the data");
        if (result.has_value()) {
            SocketCANMessage msg = result.value();
            printHexVector(msg.getFrameData());
        }
    }

    AINFO << "Sending the command control request : Disable ECU");
    // resp = canClient->sendRawMessage(GLOBAL_ES_ADDR, diagRequest);
    resp = canClient->sendRawMessage<SocketCANMessage>(GLOBAL_ES_ADDR, comControlRequest);
}

/// Tester present (keeps eyesight disabled)
void send_tester_present_message(std::unique_ptr<CANClient<SocketCANDevice>>& canClient)
{
    std::vector<uint8_t> testerPresent = {
        0x02, 0x3E, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00}; // tester present session control request
    auto resp = canClient->sendRawMessage<SocketCANMessage>(GLOBAL_ES_ADDR, testerPresent);
    AINFO << "Sending Tester Present Message results is successful : {}", resp.isSuccess());
}

void enqueue_can_data(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{

    uint64_t messageCounter = 0;
    ;
    send_disable_eyesight_ecu_can_message(canClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    while (running.load()) {

        std::vector<CANDBCMessage> messages{};
        std::lock_guard<std::mutex> lock(can_queue_mutex);

        if (frameCounter_ % 100 == 0) {
            send_tester_present_message(canClient);
        }

        auto start = std::chrono::steady_clock::now();

        while (messageCounter <= 10) {

            canClient->enQueueMessages();
            ++messageCounter;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        {
            // std::lock_guard<std::mutex> lock(can_queue_mutex);
            messages = canClient->getQueuedMessagesAndClearQueue();
        }

        for (const auto& msg : messages) {
            if (msg.name() == "ES_LKAS") {

                for (const auto& signal : msg.signals()) {
                    if (signal.name() == "COUNTER") {
                        AINFO << "Found ES_LKAS message with counter {}", signal.value());
                    }
                }
            }
        }

        auto end = std::chrono::steady_clock::now();

        // Calculate the elapsed time
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Calculate the remaining time to sleep
        // Ensure we only sleep if there's time left to sleep
        if (auto sleep_time = std::chrono::milliseconds(period_ms) - std::chrono::milliseconds(elapsed);
            sleep_time.count() > 0) {
            AINFO << "Sleeping for {} ms", sleep_time.count());
            std::this_thread::sleep_for(sleep_time);
        }

        ++frameCounter_;
        messageCounter = 0;
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);

    auto socketOps = std::make_shared<SocketOps>();
    auto canDevice = std::make_unique<SocketCANDevice>("can0", socketOps);
    auto canDatabase = CANDBC::CreateInstance();
    auto canClient = std::make_unique<CANClient<SocketCANDevice>>(std::move(canDevice), std::move(canDatabase));

    try {
        std::thread enqueue_thread(enqueue_can_data, std::ref(canClient), enqueue_period_ms);
        enqueue_thread.join();

    } catch (const std::exception& e) {
        AERROR << "Error recording CAN data: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
