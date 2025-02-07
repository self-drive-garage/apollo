#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cmath>

constexpr double MAX_STEER_RATE = 25.0; // degrees per second

constexpr int MAX_STEER_RATE_FRAMES = 7;

constexpr uint32_t STEER_STEP = 100; // how often we update the steer cmd

constexpr double STEER_MAX = 2047;

constexpr uint32_t STEER_DELTA_UP = 50; // torque increase per refresh, 0.8s to max

constexpr uint32_t STEER_DELTA_DOWN = 70; // torque decrease per refresh

constexpr uint32_t STEER_DRIVER_ALLOWANCE = 60; // allowed driver torque before start limiting

constexpr uint32_t STEER_DRIVER_MULTIPLIER = 50; // weight driver torque heavily

constexpr uint32_t STEER_DRIVER_FACTOR = 1; // from dbc

constexpr uint32_t THROTTLE_MIN = 808;

constexpr uint32_t THROTTLE_MAX = 3400;

constexpr uint32_t THROTTLE_INACTIVE = 1818; // corresponds to zero acceleration

constexpr uint32_t THROTTLE_ENGINE_BRAKE = 808; // while braking, eyesight sets throttle to this, probably for engine braking

constexpr uint32_t BRAKE_MIN = 0;

constexpr uint32_t BRAKE_MAX = 600; // about -3.5m/s2 from testing

constexpr uint32_t RPM_MIN = 0;

constexpr uint32_t RPM_MAX = 3600;

constexpr uint32_t RPM_INACTIVE = 600; // a good base rpm for zero acceleration

inline double apply_driver_steer_torque_limits(double apply_torque, double apply_torque_last, double driver_torque)
{
    // limits due to driver torque
    const auto driver_max_torque =
        STEER_MAX + (STEER_DRIVER_ALLOWANCE + driver_torque * STEER_DRIVER_FACTOR) * STEER_DRIVER_MULTIPLIER;

    const auto driver_min_torque =
        -STEER_MAX + (-STEER_DRIVER_ALLOWANCE + driver_torque * STEER_DRIVER_FACTOR) * STEER_DRIVER_MULTIPLIER;

    const auto max_steer_allowed = std::max(std::min(STEER_MAX, driver_max_torque), 0.0);

    const auto min_steer_allowed = std::min(std::max(-STEER_MAX, driver_min_torque), 0.0);

    apply_torque = std::clamp(apply_torque, min_steer_allowed, max_steer_allowed);

    // slow rate if steer torque increases in magnitude
    if (apply_torque_last > 0) {
        const auto result = std::max<double>(apply_torque_last - STEER_DELTA_DOWN, -STEER_DELTA_UP);
        apply_torque = std::clamp(apply_torque,result, apply_torque_last + STEER_DELTA_UP);
    } else {
        const auto result = std::min<double>(apply_torque_last + STEER_DELTA_DOWN, STEER_DELTA_UP);
        apply_torque = std::clamp(apply_torque,
                            apply_torque_last - STEER_DELTA_UP,
                            result);
    }

    return static_cast<int>(std::round(static_cast<float>(apply_torque)));
}
