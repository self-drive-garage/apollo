/**


there are four groups

group 1: ES_LKAS, ES_Distance // sent 280

group 2: ES_Brake, ES_Status, ES_DashStatus, ES_LKAS_State // sent 140, 2X slower than group 1

group 3: ES_Infotainment, ES_HighBeamAssist // sent 40, 7X slower than group 1

group 4: ES_STATIC_1, ES_STATIC_1 // only sent 18 messages, 15X slower than group 1

ES_HighBeamAssist
ES_DashStatus
ES_LKAS_State
ES_Infotainment
ES_STATIC_1

MAX_STEER_RATE = 25  # deg/s
MAX_STEER_RATE_FRAMES = 7  # tx control frames needed before torque can be cut

self.steer_rate_counter, apply_steer_req = \
            common_fault_avoidance(abs(CS.out.steeringRateDeg) > MAX_STEER_RATE, apply_steer_req,
                                  self.steer_rate_counter, MAX_STEER_RATE_FRAMES)

we see that MAX_STEER_RATE is being used in the fault calculations.

class CarState(CarStateBase):
  def __init__(self, CP):
    super().__init__(CP)
    can_define = CANDefine(DBC[CP.carFingerprint]["pt"])
    self.shifter_values = can_define.dv["Transmission"]["Gear"]

    self.angle_rate_calulator = CanSignalRateCalculator(50)

ret.steeringAngleDeg = cp.vl["Steering_Torque"]["Steering_Angle"]

    if not (self.CP.flags & SubaruFlags.PREGLOBAL):
      # ideally we get this from the car, but unclear if it exists. diagnostic software doesn't even have it
      ret.steeringRateDeg = self.angle_rate_calulator.update(ret.steeringAngleDeg, cp.vl["Steering_Torque"]["COUNTER"])


class CanSignalRateCalculator:
  """
  Calculates the instantaneous rate of a CAN signal by using the counter
  variable and the known frequency of the CAN message that contains it.
  """
  def __init__(self, frequency):
    self.frequency = frequency
    self.previous_counter = 0
    self.previous_value = 0
    self.rate = 0

  def update(self, current_value, current_counter):
    if current_counter != self.previous_counter:
      self.rate = (current_value - self.previous_value) * self.frequency

    self.previous_counter = current_counter
    self.previous_value = current_value

    return self.rate


 what this says to me is that the steeringAngleDeg is read from the CAN messages on the bus, sepcifically the
Steering_Torque sensor.

then it is compared with the MAX_STEER_RATE, and then that's used to decide on the EPS faults.

let's update the c++ code to handle this.  for the steeringAngleDeg, since we have a function that reads messages from
the enqued messages, let's have a static variable for now called steeringAngleDeg and we updated it each time it's found
in the enqued messages.  then we can use that to calcualte the eps faults



*/

#include "platform/io/sockets/SocketOps.hpp"
#include "platform/io/can/CANClient.hpp"
#include "platform/io/can/SocketCANDevice.hpp"
#include "platform/io/can/SocketCANMessage.hpp"
#include "platform/io/can/dbc/CANDBC.hpp"

#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
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
std::mutex send_message_mutex;

std::atomic<bool> running(true);
int enqueue_period_ms = 20; // 50 Hz based on message frequency

const double MAX_STEER_RATE = 25.0; // degrees per second
const int MAX_STEER_RATE_FRAMES = 7;

class CanSignalRateCalculator
{
public:
    CanSignalRateCalculator(double frequency)
        : frequency(frequency)
        , previous_counter(0)
        , previous_value(0)
        , rate(0)
    {}

    double update(double current_value, int current_counter)
    {
        if (current_counter != previous_counter) {
            rate = (current_value - previous_value) * frequency;
        }

        previous_counter = current_counter;
        previous_value = current_value;

        return rate;
    }

private:
    double frequency;
    int previous_counter;
    double previous_value;
    double rate;
};

double steeringAngleDeg = 0.0;
int steeringCounter = 0;

void updateSteeringAngle(const CANDBCMessage& msg)
{
    for (const auto& signal : msg.signals()) {
        if (signal.name() == "Steering_Angle") {
            steeringAngleDeg = signal.value();
        } else if (signal.name() == "COUNTER") {
            steeringCounter = static_cast<int>(signal.value());
        }
    }
}

std::shared_ptr<arrow::UInt32Builder> address_builder = std::make_shared<arrow::UInt32Builder>();
std::shared_ptr<arrow::StringBuilder> name_builder = std::make_shared<arrow::StringBuilder>();
std::shared_ptr<arrow::StringBuilder> canbus_builder = std::make_shared<arrow::StringBuilder>();
std::shared_ptr<arrow::StringBuilder> signal_name_builder = std::make_shared<arrow::StringBuilder>();
std::shared_ptr<arrow::DoubleBuilder> signal_value_builder = std::make_shared<arrow::DoubleBuilder>();
std::shared_ptr<arrow::UInt64Builder> timestamp_builder = std::make_shared<arrow::UInt64Builder>();

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

void write_parquet_file(const std::string& filename, const std::shared_ptr<arrow::Table>& table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(filename));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 3));
}

void write_incremental_parquet(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(can_queue_mutex);

    std::shared_ptr<arrow::Array> address_array;
    std::shared_ptr<arrow::Array> name_array;
    std::shared_ptr<arrow::Array> canbus_array;
    std::shared_ptr<arrow::Array> signal_name_array;
    std::shared_ptr<arrow::Array> signal_value_array;
    std::shared_ptr<arrow::Array> timestamp_array;

    PARQUET_THROW_NOT_OK(address_builder->Finish(&address_array));
    PARQUET_THROW_NOT_OK(name_builder->Finish(&name_array));
    PARQUET_THROW_NOT_OK(canbus_builder->Finish(&canbus_array));
    PARQUET_THROW_NOT_OK(signal_name_builder->Finish(&signal_name_array));
    PARQUET_THROW_NOT_OK(signal_value_builder->Finish(&signal_value_array));
    PARQUET_THROW_NOT_OK(timestamp_builder->Finish(&timestamp_array));

    const auto schema = arrow::schema({arrow::field("address", arrow::uint32()),
                                       arrow::field("name", arrow::utf8()),
                                       arrow::field("canbus", arrow::utf8()),
                                       arrow::field("signal_name", arrow::utf8()),
                                       arrow::field("signal_value", arrow::float64()),
                                       arrow::field("timestamp", arrow::uint64())});

    auto table = arrow::Table::Make(
        schema, {address_array, name_array, canbus_array, signal_name_array, signal_value_array, timestamp_array});
    write_parquet_file(filename, table);

    address_builder->Reset();
    name_builder->Reset();
    canbus_builder->Reset();
    signal_name_builder->Reset();
    signal_value_builder->Reset();
    timestamp_builder->Reset();
}

void record_can_data(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, const std::string& filename)
{
    while (running.load()) {
        std::vector<CANDBCMessage> messages;

        {
            std::lock_guard<std::mutex> lock(can_queue_mutex);
            messages = canClient->getQueuedMessagesAndClearQueue();
        }

        for (const auto& msg : messages) {
            if (msg.name() == "Steering_Torque") {
                updateSteeringAngle(msg);
                AINFO <<
                    "pulled {} messages from the queue and steering angle is", messages.size(), steeringAngleDeg);
            }
            for (const auto& signal : msg.signals()) {
                PARQUET_THROW_NOT_OK(address_builder->Append(msg.address()));
                PARQUET_THROW_NOT_OK(name_builder->Append(msg.name()));
                PARQUET_THROW_NOT_OK(canbus_builder->Append(msg.getCanBus())); // CANBus enum to string conversion
                PARQUET_THROW_NOT_OK(signal_name_builder->Append(signal.name()));
                PARQUET_THROW_NOT_OK(signal_value_builder->Append(signal.value()));
                PARQUET_THROW_NOT_OK(timestamp_builder->Append(signal.timestampNanoSeconds()));
            }
        }

        if (messages.size() > 0) {
            write_incremental_parquet(filename);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void handle_signal(int signal)
{
    if (signal == SIGINT) {
        running.store(false);
    }
}

void enqueue_can_data(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{
    while (running.load()) {
        auto start = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lock(can_queue_mutex);
            canClient->enQueueMessages();
        }
        auto end = std::chrono::steady_clock::now();
        std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
    }
}

void create_and_send_message(std::unique_ptr<CANClient<SocketCANDevice>>& canClient,
                             const std::string& name,
                             const std::unordered_map<std::string, double>& signalValueMap)
{
    const auto msg = canClient->createCANDBCMessage(name, signalValueMap);

    std::lock_guard<std::mutex> lock(send_message_mutex);

    canClient->sendMessage<SocketCANMessage>(msg.value());
}

/**
 ***************************************************
 *                  GROUP 1 MESSAGES
 ***************************************************
 */
inline void send_es_lkas_message(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter, int lkas_output, int steering_toruqe_output)
{
    counter = counter % 0x10;
    create_and_send_message(
        canClient, "ES_LKAS", {{"LKAS_Request", 1}, {"LKAS_Output", lkas_output}, {"SET_1", 1}, {"COUNTER", counter}});

    // create_and_send_message(canClient, "Steering_Torque_2", {
    //     {"Steer_Torque_Output", steering_toruqe_output},
    //     {"Signal1" , 7},
    //     {"Steer_Torque_Sensor" , 6},
    //     {"Steering_Disabled" , 0},
    // {"Steering_Active" , 1},
    // });
}

inline void send_es_distance(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(canClient,
                            "ES_Distance",
                            {{"Signal6", 0},
                             {"Cruise_Resume", 0},
                             {"Cruise_Set", 0},
                             {"Cruise_Cancel", 0},
                             {"Signal5", 0},
                             {"Close_Distance", 0},
                             {"Signal4", 1},
                             {"Cruise_EPB", 0},
                             {"Distance_Swap", 0},
                             {"Cruise_Brake_Active", 0},
                             {"Signal7", 0},
                             {"Cruise_Soft_Disable", 1},
                             {"Low_Speed_Follow", 1},
                             {"Car_Follow", 1},
                             {"Signal1", 2},
                             {"Cruise_Throttle", 808},
                             {"Cruise_Fault", 0},
                             {"Signal1", 0},
                             {"COUNTER", counter}});
}

/**
 ***************************************************
 *                  GROUP 2 MESSAGES
 *                  GROUP 2 is 2X slower than GROUP 1
 ***************************************************
 */

inline void send_es_brake(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(canClient,
                            "ES_Brake",
                            {{"Signal3", 0},
                             {"Cruise_Activated", 0},
                             {"Cruise_Brake_Active", 0},
                             {"Cruise_Brake_Fault", 0},
                             {"Cruise_Brake_Lights", 0},
                             {"AEB_Status", 0},
                             {"Signal1", 0},
                             {"COUNTER", counter}});
}

inline void send_es_status(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(canClient,
                            "ES_Status",
                            {{"Signal3", 0},
                             {"Cruise_Hold", 0},
                             {"Brake_Lights", 0},
                             {"Cruise_Activated", 0},
                             {"Signal2", 0},
                             {"Cruise_RPM", 100},
                             {"Cruise_Fault", 0},
                             {"Signal1", 0},
                             {"COUNTER", counter}});
}

inline void send_es_dash_status(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    counter = counter % 0x10;
    create_and_send_message(canClient,
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
                             {"Cruise_Activated", 0},
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
                             {"PCB_Off", 0},
                             {"COUNTER", counter}});
}

inline void send_es_lkas_state(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    counter = counter % 0x10;
    create_and_send_message(canClient,
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

/**
 ***************************************************
 *                  GROUP 3 MESSAGES
 *                  GROUP 3 is 7X slower than GROUP 1
 ***************************************************
 */

void send_es_highbeam_assist(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(canClient, "ES_HighBeamAssist", {{"HBA_Available", 1}, {"COUNTER", counter}});
}

void send_es_infotainment(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(
        canClient,
        "ES_Infotainment",
        {{"Signal2", 0}, {"LKAS_State_Infotainment", 0}, {"Signal1", 0}, {"LKAS_Blue_Lines", 1}, {"COUNTER", counter}});
}

/**
 ***************************************************
 *                  GROUP 4 MESSAGES
 *                  GROUP 4 is 18X slower than GROUP 1
 ***************************************************
 */
void send_es_static1(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{

    counter = counter % 0x10;
    create_and_send_message(canClient, "ES_STATIC_1", {{"SET_3", 0}, {"COUNTER", counter}});
}
void send_es_static2(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int counter)
{
    counter = counter % 0x10;
    create_and_send_message(canClient, "ES_STATIC_2", {{"SET_3", 0}, {"COUNTER", counter}});
}

void send_group_1_messages(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{
    int counter = 0;
    double lkas_output = 0.0;
    const double STEER_MAX = 2047/4;
    const int STEER_STEP = 10;
    int steer_rate_counter = 0;
    bool apply_steer_req = true;
    double steer_value = 0.0;
    double increment_value = 0.001; // Adjust this value for smoother transitions

    CanSignalRateCalculator angle_rate_calculator(50); // Example frequency, adjust as needed

    while (running.load()) {
        auto start = std::chrono::steady_clock::now();

        // Calculate the steering rate
        double steeringRateDeg = angle_rate_calculator.update(steeringAngleDeg, steeringCounter);
        AINFO << "calcualted steering rate deg as {}", steeringRateDeg);
        // Fault prevention logic
        if (abs(steeringRateDeg) > MAX_STEER_RATE) {
            if (++steer_rate_counter > MAX_STEER_RATE_FRAMES) {
                apply_steer_req = false;
            }
        } else {
            steer_rate_counter = 0;
            apply_steer_req = true;
        }

        // Update torque every STEER_STEP iterations
        if (counter % STEER_STEP == 0 && apply_steer_req) {
            // Gradually adjust steer_value within the range [-1.0, 1.0]
            steer_value += increment_value;
            if (steer_value > 1.0 || steer_value < -1.0) {
                increment_value = -increment_value; // Reverse direction when limits are reached
                steer_value += increment_value;     // Apply the reversed increment to stay within limits
            }

            // Calculate LKAS_Output
            lkas_output = steer_value * STEER_MAX;

            // Ensure LKAS_Output is within safe limits
            lkas_output = std::max(-STEER_MAX, std::min(STEER_MAX, lkas_output));
        }

        // Send ES_LKAS message
        if (apply_steer_req) {
            AINFO << "applying lkas output {} with steeringAngleDeg = {}", lkas_output, steeringAngleDeg);
            send_es_lkas_message(canClient, counter, static_cast<int>(lkas_output), lkas_output);
        } else {
            AINFO << "FAULT DETECTED ... NOT APPLYING {}", lkas_output);
            // Send ES_LKAS message with LKAS_Request set to 0 to indicate no steering request
            send_es_lkas_message(canClient, counter, 0, 0);
        }

        // Send ES_Distance message
        send_es_distance(canClient, counter);

        ++counter;

        std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
    }
}

// group 2: 2X slower than group 1
// group 2: ES_Brake, ES_Status, ES_DashStatus, ES_LKAS_State // sent 140
void send_group_2_messages(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{
    int counter = 0;
    while (running.load()) {
        auto start = std::chrono::steady_clock::now();

        send_es_brake(canClient, counter);
        send_es_status(canClient, counter);
        send_es_dash_status(canClient, counter);
        send_es_lkas_state(canClient, counter);

        ++counter;

        std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
    }
}

// group 3: 7X slower than group 1
// group 3: ES_Infotainment, ES_HighBeamAssist
void send_group_3_messages(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{
    int counter = 0;
    while (running.load()) {
        auto start = std::chrono::steady_clock::now();

        send_es_infotainment(canClient, counter);
        send_es_highbeam_assist(canClient, counter);

        ++counter;

        std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
    }
}

// group 4: 15X slower than group 1
// group 4: ES_Brake, ES_Status, ES_DashStatus, ES_LKAS_State // sent 140
void send_group_4_messages(std::unique_ptr<CANClient<SocketCANDevice>>& canClient, int period_ms)
{
    int counter = 0;
    while (running.load()) {
        auto start = std::chrono::steady_clock::now();

        send_es_static1(canClient, counter);
        send_es_static2(canClient, counter);
        ++counter;

        std::this_thread::sleep_until(start + std::chrono::milliseconds(period_ms));
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);

    auto socketOps = std::make_shared<SocketOps>();
    auto canDevice = std::make_unique<SocketCANDevice>("can0", socketOps);
    auto canDatabase = CANDBC::CreateInstance();
    auto canClient = std::make_unique<CANClient<SocketCANDevice>>(std::move(canDevice), std::move(canDatabase));

    const std::string filename = getHomeDirectory() + "/solo/can_data/" + getCurrentDateTimeForFilename() + ".parquet";

    try {
        std::thread enqueue_thread(enqueue_can_data, std::ref(canClient), enqueue_period_ms);
        std::thread record_thread(record_can_data, std::ref(canClient), filename);

        // group 1: running at 100 HZ
        std::thread send_group1_thread(send_group_1_messages, std::ref(canClient), enqueue_period_ms);
        // group 2: is 2X slower than group 1
        std::thread send_group2_thread(send_group_2_messages, std::ref(canClient), enqueue_period_ms * 2);
        // group 3: is 7X slower than group 1
        std::thread send_group3_thread(send_group_3_messages, std::ref(canClient), enqueue_period_ms * 7);
        // group 4: is 15X slower than group 2
        std::thread send_group4_thread(send_group_4_messages, std::ref(canClient), enqueue_period_ms * 15);

        enqueue_thread.join();
        record_thread.join();

        send_group1_thread.join();
        send_group2_thread.join();
        send_group3_thread.join();
        send_group4_thread.join();

    } catch (const std::exception& e) {
        AERROR << "Error recording CAN data: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
