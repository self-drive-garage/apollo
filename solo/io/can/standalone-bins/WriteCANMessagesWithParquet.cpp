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
std::atomic<bool> running(true);
int enqueue_period_ms = 1; // 100 Hz by default

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
            AINFO << "pulled {} messages from the queue", messages.size());
        }

        for (const auto& msg : messages) {
            if (msg.name() == "ES_LKAS") {

                for (const auto& signal : msg.signals()) {
                    if (signal.name() == "COUNTER") {
                        AINFO << "Found ES_LKAS message with counter {}", signal.value());
                    }
                }
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

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

        // Calculate the elapsed time
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Calculate the remaining time to sleep
        auto sleep_time = std::chrono::milliseconds(period_ms) - std::chrono::milliseconds(elapsed);

        // Ensure we only sleep if there's time left to sleep
        if (sleep_time.count() > 0) {
            std::this_thread::sleep_for(sleep_time);
        }
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);

    auto socketOps = std::make_shared<SocketOps>();
    auto canDevice = std::make_unique<SocketCANDevice>("vcan0", socketOps);
    auto canDatabase = CANDBC::CreateInstance();
    auto canClient = std::make_unique<CANClient<SocketCANDevice>>(std::move(canDevice), std::move(canDatabase));

    const std::string filename = getHomeDirectory()+"/solo/can_data/" + getCurrentDateTimeForFilename() + ".parquet";

    try {
        std::thread enqueue_thread(enqueue_can_data, std::ref(canClient), enqueue_period_ms);
        std::thread record_thread(record_can_data, std::ref(canClient), filename);

        enqueue_thread.join();
        record_thread.join();
    } catch (const std::exception& e) {
        AERROR << "Error recording CAN data: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
