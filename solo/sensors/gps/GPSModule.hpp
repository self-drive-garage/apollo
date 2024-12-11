#pragma once

#include "platform/sensors/gps/GPSSensor.hpp"
#include "platform/sensors/gps/idl/GPSData.hpp"
#include "platform/transport/Transport.hpp"
#include "platform/transport/TransportPublisher.hpp"

#include "platform/TemporaryGlobals.hpp"

#include <memory>
#include <thread>

namespace platform::sensors {

class GPSModule {
public:
    GPSModule(std::unique_ptr<GPSSensor> gpsSensor)
        : gpsSensor_{std::move(gpsSensor)},
          publisher_{transport::Transport::CreatePublisher<GPSData>("GPSData")}
    {}

    ~GPSModule() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    };

    void initialize() {
        auto status = gpsSensor_->initialize();
        if (!status.ok()) {
            throw std::runtime_error("Failed to initialize GPS sensor");
        }
        status = gpsSensor_->open();
        if (!status.ok()) {
            throw std::runtime_error("Failed to open GPS sensor");
        }

        status = gpsSensor_->calibrate();
        if (!status.ok()) {
            throw std::runtime_error("Failed to calibrate GPS sensor");
        }

        running_ = true;
        thread_ = std::thread(&GPSModule::run, this);
    };

    void run() {
        while (running_) {
            auto now = std::chrono::steady_clock::now();

            auto [status, data] = gpsSensor_->read();
            if (status.ok() && data) {
                publisher_->publish(data.value());
            }

            auto elapsed = std::chrono::steady_clock::now() - now;
            std::this_thread::sleep_for(TEMPORARY_GLOBALS_UPDATE_INTERVAL - elapsed);
        }
    };

private:
    std::unique_ptr<GPSSensor> gpsSensor_;
    std::shared_ptr<transport::TransportPublisher<GPSData>> publisher_;
    std::thread thread_;
    std::atomic<bool> running_{false};
};

} // namespace platform::sensors
