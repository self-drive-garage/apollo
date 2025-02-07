#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

constexpr double MAX_STEER_RATE = 25.0; // degrees per second

constexpr int MAX_STEER_RATE_FRAMES = 7;

constexpr uint32_t STEER_STEP = 2; // how often we update the steer cmd

constexpr double_t STEER_MAX = 2047; // Signed integer range

constexpr double_t STEER_DELTA_UP = 50; // torque increase per refresh, 0.8s to max

constexpr double_t STEER_DELTA_DOWN = 70; // torque decrease per refresh

constexpr double_t STEER_DRIVER_ALLOWANCE = 60; // allowed driver torque before start limiting

constexpr double_t STEER_DRIVER_MULTIPLIER = 50; // weight driver torque heavily

constexpr double_t STEER_DRIVER_FACTOR = 1; // from dbc

inline double_t apply_driver_steer_torque_limits(double_t currentTorqueValue,
                                            const double_t previousTorqueValue,
                                            const double steeringTorqueMeasured)
{
    // Limits due to driver torque
    const double_t driver_max_torque =
        STEER_MAX + (STEER_DRIVER_ALLOWANCE + steeringTorqueMeasured * STEER_DRIVER_FACTOR) *
                        STEER_DRIVER_MULTIPLIER;

    const double_t driver_min_torque =
        -STEER_MAX + (-STEER_DRIVER_ALLOWANCE + steeringTorqueMeasured * STEER_DRIVER_FACTOR) *
                         STEER_DRIVER_MULTIPLIER;

    const double_t max_steer_allowed = std::max(std::min(STEER_MAX, driver_max_torque), 0.0);
    const double_t min_steer_allowed = std::min(std::max(-STEER_MAX, driver_min_torque), 0.0);

    currentTorqueValue = std::clamp(currentTorqueValue, min_steer_allowed, max_steer_allowed);

    // Slow rate if steer torque increases in magnitude
    if (previousTorqueValue > 0) {
        const double_t new_torque = std::max(previousTorqueValue - STEER_DELTA_DOWN, -STEER_DELTA_UP);
        currentTorqueValue = std::clamp(currentTorqueValue, new_torque, previousTorqueValue + STEER_DELTA_UP);
    } else {
        const double_t new_torque = std::min(previousTorqueValue + STEER_DELTA_DOWN, STEER_DELTA_UP);
        currentTorqueValue = std::clamp(currentTorqueValue, previousTorqueValue - STEER_DELTA_UP, new_torque);
    }

    return currentTorqueValue;
}
