
#include "platform/io/sockets/SocketOps.hpp"
#include "platform/io/can/CANClient.hpp"
#include "platform/io/can/SocketCANDevice.hpp"
#include "platform/io/can/SocketCANMessage.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"
#include "platform/io/can/standalone-bins/CanSignalRateCalculator.hpp"
#include "platform/io/can/standalone-bins/LIMITS.hpp"

#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace platform::io;
using namespace platform::io::can;
using namespace platform::io::can::dbc;

struct SteeringControlValues
{
    bool shouldApplySteering = true;
    double_t previousTorqueValue = 0;
    double_t currentTorqueValue = 0;
    int32_t numberSteeringLimitViolations = 0;
    uint32_t counter = 0;
    double increment_value = 0.1; // Adjust this value for smoother transitions
};

uint64_t frame_ = 0;
constexpr uint32_t rate_keeper_ = 10; // 100 Hz based on message frequency
std::mutex can_queue_mutex;
std::atomic<bool> running(true);
uint32_t GLOBAL_ES_ADDR = 0x787;

// 50 Hz is the known frequency of the steering torque sensor
CanSignalRateCalculator angle_rate_calculator(50);

std::unordered_map<std::string, std::vector<std::vector<uint8_t>>> rawSentMessages{};

std::string getCurrentDateTimeForFilename()
{
    auto now = std::time(nullptr);
    std::tm localTime;
    localtime_r(&now, &localTime);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

std::string getHomeDirectory()
{
    const char* homeDir;
    if ((homeDir = std::getenv("HOME")) == nullptr) {
        homeDir = getpwuid(getuid())->pw_dir;
    }
    return std::string(homeDir);
}

std::string hexVectorToString(const std::vector<uint8_t>& vec)
{
    std::ostringstream oss;
    for (const auto& byte : vec) {
        // Print each byte as a two-digit hexadecimal value
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    return oss.str();
}

void writeToCsvFile(const std::string& filename)
{
    // Open the file in write mode
    std::ofstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing");
    }

    // Write CSV header
    file << "Name,HexData" << std::endl;

    // Write each name and corresponding hex data string
    for (const auto& [name, rawDataVectors] : rawSentMessages) {

        for (const auto& rawData : rawDataVectors) {
            const std::string hexData = hexVectorToString(rawData);
            file << name << "," << hexData << std::endl;
        }
    }

    // Close the file
    file.close();
}

void handle_signal(int signal)
{
    if (signal == SIGINT) {
        running.store(false);
        AINFO << "Received SIGINT.  Stopping CAN Messages and writing out the raw messages sent");
        const std::string filename = getHomeDirectory() + "/solo/can_data/rotate_steering_wheel_raw_messages-" +
                                     getCurrentDateTimeForFilename() + ".csv";
        writeToCsvFile(filename);
    }
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
                std::cout << hexVectorToString(msg.getFrameData()) << std::endl;
            }
        }
    } else {
        AINFO << "We received data on the original GLOBAL_ES_ADDR.  Logging the data");
        if (result.has_value()) {
            SocketCANMessage msg = result.value();
            std::cout << hexVectorToString(msg.getFrameData()) << std::endl;
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
    AINFO << "Sending Tester Present Message results is successful {}: ", resp.isSuccess());
}

inline CANDBCMessage CreateMessage(std::unique_ptr<CANClient<SocketCANDevice>>& canClient,
                                   const std::string& name,
                                   const std::unordered_map<std::string, double>& signalValueMap)
{
    auto message = canClient->createCANDBCMessage(name, signalValueMap);
    return message.value();
}

CANDBCMessage ReadSteeringTorqueMessage(std::unique_ptr<CANClient<SocketCANDevice>>& canClient)
{
    while (true) {
        canClient->enQueueMessages();

        for (std::vector<CANDBCMessage> messages = canClient->getQueuedMessagesAndClearQueue();
             const auto& msg : messages) {
            if (msg.name() == "Steering_Torque") {
                return msg;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

// TODO: check that ES_LKAS LKAS Output is big-endian
inline CANDBCMessage CreateSteeringControlMessage(std::unique_ptr<CANClient<SocketCANDevice>>& canClient,
                                                  SteeringControlValues& steeringControlValues)
{
    double_t steeringTorqueMeasured = 0;
    double steeringAngleMeasured = 0.0;
    double steeringAngleRate = 0.0;
    int steeringTorqueMessageCounter = 0;

    steeringControlValues.counter = steeringControlValues.counter % 0x10;

    {
        std::lock_guard<std::mutex> lock(can_queue_mutex);

        for (auto steeringTorqueMessage = ReadSteeringTorqueMessage(canClient);
             const auto& signal : steeringTorqueMessage.signals()) {
            if (signal.name() == "Steer_Torque_Sensor") {
                steeringTorqueMeasured = signal.value();
            } else if (signal.name() == "Steering_Angle") {
                steeringAngleMeasured = signal.value();
            } else if (signal.name() == "COUNTER") {
                steeringTorqueMessageCounter = static_cast<int>(signal.value());
            }
        }
    }

    steeringControlValues.currentTorqueValue =
        steeringControlValues.previousTorqueValue + steeringControlValues.increment_value;

    steeringControlValues.currentTorqueValue = apply_driver_steer_torque_limits(
        steeringControlValues.currentTorqueValue, steeringControlValues.previousTorqueValue, steeringTorqueMeasured);

    // torque sensor does not give a rate of change for steering angle
    // so we use the message counter to calculate it
    steeringAngleRate = angle_rate_calculator.update(steeringAngleMeasured, steeringTorqueMessageCounter);

    AINFO <<
        "Torque Sensor Output: Steering Torque : {}, Steering Angle: {}, Calculated Steering Angle Rate : {} deg/sec",
        steeringTorqueMeasured,
        steeringAngleMeasured,
        steeringAngleRate);

    // Fault prevention logic
    if (abs(steeringAngleRate) > MAX_STEER_RATE && steeringControlValues.shouldApplySteering) {
        AINFO << "exceeded steering angle rate of {}", MAX_STEER_RATE);
        ++steeringControlValues.numberSteeringLimitViolations;
    } else {
        steeringControlValues.numberSteeringLimitViolations = 0;
    }

    if (steeringControlValues.numberSteeringLimitViolations > MAX_STEER_RATE_FRAMES) {
        AINFO << "exceeded number of allowed violations of {}", MAX_STEER_RATE);
        AINFO << "we disable steering messages for a few frames to prevent a EPS fault");
        steeringControlValues.shouldApplySteering = false;
    }

    return CreateMessage(canClient,
                         "ES_LKAS",
                         {{"LKAS_Request", steeringControlValues.shouldApplySteering},
                          {"LKAS_Output", static_cast<int>(steeringControlValues.currentTorqueValue)},
                          {"SET_1", 1},
                          {"COUNTER", steeringControlValues.counter}});
}

inline CANDBCMessage CreateESDistance(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    return CreateMessage(canClient,
                         "ES_Distance",
                         {{"Signal6", 0},
                          {"Cruise_Resume", 1},
                          {"Cruise_Set", 1},
                          {"Cruise_Cancel", 0},
                          {"Signal5", 0},
                          {"Close_Distance", 0},
                          {"Signal4", 1},
                          {"Cruise_EPB", 0},
                          {"Distance_Swap", 0},
                          {"Cruise_Brake_Active", 0},
                          {"Signal7", 0},
                          {"Cruise_Soft_Disable", 0},
                          {"Low_Speed_Follow", 1},
                          {"Car_Follow", 1},
                          {"Signal1", 2},
                          {"Cruise_Throttle", 0},
                          {"Cruise_Fault", 0},
                          {"Signal1", 0},
                          {"COUNTER", counter}});
}

inline CANDBCMessage CreateESBrake(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    return CreateMessage(canClient,
                         "ES_Brake",
                         {{"Signal3", 0},
                          {"Cruise_Activated", 1},
                          {"Cruise_Brake_Active", 0},
                          {"Cruise_Brake_Fault", 0},
                          {"Cruise_Brake_Lights", 0},
                          {"AEB_Status", 0},
                          {"Signal1", 0},
                          {"Brake_Pressure", 0},
                          {"COUNTER", counter}});
}

inline CANDBCMessage CreateESStatus(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    return CreateMessage(canClient,
                         "ES_Status",
                         {{"Signal3", 0},
                          {"Cruise_Hold", 0},
                          {"Brake_Lights", 0},
                          {"Cruise_Activated", 1},
                          {"Signal2", 0},
                          {"Cruise_RPM", 0},
                          {"Cruise_Fault", 0},
                          {"Signal1", 0},
                          {"COUNTER", counter}});
}

inline CANDBCMessage CreateESDasStatusMessage(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    counter = counter % 0x10;
    return CreateMessage(canClient,
                         "ES_DashStatus",
                         {{"Cruise_State", 0},
                          {"Far_Distance", 0},
                          {"Signal7", 0},
                          {"Car_Follow", 0},
                          {"Brake_Lights", 0},
                          {"Display_Own_Car", 1},
                          {"Cruise_On", 0},
                          {"Cruise_Fault", 0},
                          {"Cruise_Set_Speed", 0},
                          {"Signal6", 0},
                          {"Cruise_Activated", 1},
                          {"Cruise_Disengaged", 0},
                          {"Signal5", 0},
                          {"Conventional_Cruise", 0},
                          {"Signal4", 1},
                          {"Cruise_Distance", 0},
                          {"Signal3", 0},
                          {"Cruise_Status_Msg", 0},
                          {"Cruise_Soft_Disable", 0},
                          {"Signal2", 0},
                          {"LKAS_State_Msg", 0},
                          {"Cruise_State_Msg", 0},
                          {"Signal1", 0},
                          {"LDW_Off", 0},
                          {"PCB_Off", 1},
                          {"COUNTER", counter}});
}

inline CANDBCMessage CreateESLKASState(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    return CreateMessage(canClient,
                         "ES_LKAS_State",
                         {{"Signal3", 1048976},
                          {"LKAS_Alert", 0},
                          {"LKAS_Right_Line_Visible", 1},
                          {"LKAS_Left_Line_Visible", 1},
                          {"LKAS_Right_Line_Light_Blink", 0},
                          {"LKAS_Right_Line_Enable", 0},
                          {"LKAS_Left_Line_Light_Blink", 0},
                          {"LKAS_Left_Line_Enable", 0},
                          {"Backward_Speed_Limit_Menu", 0},
                          {"Signal2", 0},
                          {"LKAS_Dash_State", 2},
                          {"LKAS_ACTIVE", 1},
                          {"Signal1", 0},
                          {"LKAS_Alert_Msg", 0},
                          {"COUNTER", counter}});
}

inline CANDBCMessage CreateESHighbeamAssist(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    return CreateMessage(canClient, "ES_HighBeamAssist", {{"HBA_Available", 0}, {"COUNTER", counter}});
}

inline CANDBCMessage CreateESStatic1(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    return CreateMessage(canClient, "ES_STATIC_1", {{"SET_3", 3}, {"COUNTER", counter}});
}

inline CANDBCMessage CreateESStatic2(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    counter = counter % 0x10;
    return CreateMessage(canClient, "ES_STATIC_2", {{"SET_3", 3}, {"COUNTER", counter}});
}

void Thread_RotateSteeringWheel(std::unique_ptr<CANClient<SocketCANDevice>>& canClient)
{

    SteeringControlValues steeringControlValues;

    send_disable_eyesight_ecu_can_message(canClient);

    while (running.load()) {

        auto start = std::chrono::steady_clock::now();

        std::vector<CANDBCMessage> messages{};
        messages.reserve(20);

        // steering message
        if (frame_ % STEER_STEP == 0) {
            messages.push_back(CreateSteeringControlMessage(canClient, steeringControlValues));
            steeringControlValues.previousTorqueValue = steeringControlValues.currentTorqueValue;
        }

        if (frame_ % 2 == 0) {
            messages.push_back(CreateESStatic2(canClient, frame_ / 5));
        }

        if (frame_ % 5 == 0) {
            messages.push_back(CreateESStatus(canClient, frame_ / 5));
            messages.push_back(CreateESBrake(canClient, frame_ / 5));
            messages.push_back(CreateESDistance(canClient, frame_ / 5));

            messages.push_back(CreateESHighbeamAssist(canClient, frame_ / 5));
        }

        // es dash status
        if (frame_ % 10 == 0) {
            messages.push_back(CreateESDasStatusMessage(canClient, frame_ / 10));
            messages.push_back(CreateESLKASState(canClient, frame_ / 10));

            messages.push_back(CreateESStatic1(canClient, frame_ / 5));
        }

        if (frame_ % 100 == 0) {
            send_tester_present_message(canClient);
        }

        for (const auto& message : messages) {
            if (auto [status, rawData] = canClient->sendMessage<SocketCANMessage>(message); rawData.has_value()) {
                rawSentMessages[message.name()].push_back(rawData.value());
            }
        }

        // we aim to maintain a rate of 100 Hz
        std::this_thread::sleep_until(start + std::chrono::milliseconds(rate_keeper_));

        ++frame_;
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

        // group 1: running at 100 HZ
        std::thread thread_RotateSteeringWheel(Thread_RotateSteeringWheel, std::ref(canClient));
        thread_RotateSteeringWheel.join();

    } catch (const std::exception& e) {
        AERROR << "Error recording CAN data: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
