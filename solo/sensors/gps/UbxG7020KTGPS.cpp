#include "platform/sensors/gps/UbxG7020KTGPS.hpp"
#include "platform/io/Status.hpp"

#include <spdlog/spdlog.h>
#include <thread>
#include <cmath>

namespace platform::sensors {

UbxG7020KTGPS::UbxG7020KTGPS()
    : gpsmmReceiver_{"localhost", DEFAULT_GPSD_PORT}
    , currentData_{}
{
    open();
}

io::Status UbxG7020KTGPS::open() {
   // Attempt to start streaming GPS data
    AINFO << "Opening GPS device...");
    if (gpsmmReceiver_.stream(WATCH_ENABLE | WATCH_JSON) == nullptr) {
        AERROR << "Failed to connect to GPSD.");
        return io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::ERROR_OPENING_DEVICE, "Failed to connect to GPSD."};
    }

    // Wait for valid GPS data
    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        ADEBUG << "Waiting for GPS device to provide valid data...");
        if (gpsmmReceiver_.waiting(WAIT_TIME.count() * 1000000)) {
            ADEBUG << "GPS device detected, checking for valid data...");
            struct gps_data_t* data = gpsmmReceiver_.read();
            // Check if we have a 2D or 3D fix
            if (data != nullptr && (data->fix.mode == MODE_2D || data->fix.mode == MODE_3D)) {
                ADEBUG << "GPS device detected and providing valid data.");
                return io::Status{platform::io::STATUS::SUCCESS};
            }
        }
        
        std::this_thread::sleep_for(WAIT_TIME);
    }

    // If we couldn't get a valid fix after MAX_ATTEMPTS, consider it a failure
    AERROR << "GPS device not detected or not providing valid data. Is it plugged in???");
    return io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::SENSOR_FAILURE, "GPS device not detected or not providing valid data. Is it plugged in?"};
}

std::tuple<io::Status, std::optional<GPSData>> UbxG7020KTGPS::read(std::chrono::milliseconds timeout) {

    // Wait for data to be available
    if (!gpsmmReceiver_.waiting(timeout.count() * 1000)) {  // waiting() expects microseconds
        return std::make_tuple(io::Status{platform::io::STATUS::ERROR, platform::io::ERROR::TIMEOUT}, std::nullopt);
    }

    // Read GPS data
    struct gps_data_t* gps_data = gpsmmReceiver_.read();

    if (gps_data == nullptr) {
        // If no new data, return the last valid data if available
        // if (currentData_.valid) {
        //     return std::make_tuple(io::Status{platform::io::STATUS::SUCCESS}, currentData_);
        // } 
        return std::make_tuple(io::Status{platform::io::STATUS::ERROR}, std::nullopt);
    }

    // Check if we have a valid 2D or 3D fix
    if (gps_data->fix.mode == MODE_2D || gps_data->fix.mode == MODE_3D) {
        // Ensure latitude and longitude are valid numbers
        if (!std::isnan(gps_data->fix.latitude) && !std::isnan(gps_data->fix.longitude)) {
            // Update currentData_ with new GPS information
            currentData_.latitude(gps_data->fix.latitude);
            currentData_.longitude(gps_data->fix.longitude);
            currentData_.altitude(gps_data->fix.altitude);
            currentData_.speed(gps_data->fix.speed);
            currentData_.track(gps_data->fix.track);
            currentData_.climb(gps_data->fix.climb);

            auto timestamp = static_cast<int64_t>(gps_data->fix.time.tv_sec) * 1000000000LL + gps_data->fix.time.tv_nsec;
            currentData_.timestamp_nanoseconds(timestamp);
            currentData_.status(gps_data->fix.status);
            currentData_.valid(true);

            return std::make_tuple(io::Status{platform::io::STATUS::SUCCESS}, currentData_);
        }
    }

    // If we couldn't get valid data, mark currentData_ as invalid
    currentData_.valid(false);
    return std::make_tuple(io::Status{platform::io::STATUS::ERROR}, std::nullopt);
}

io::Status UbxG7020KTGPS::close() {    
    currentData_ = GPSData{};
    return io::Status{platform::io::STATUS::SUCCESS};
}

UbxG7020KTGPS::~UbxG7020KTGPS() {
    close();
}

} // namespace platform::sensors
