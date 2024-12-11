/// @file
/// @legal Copyright (C) 2024 ....... - All Rights Reserved
/// @brief Base test fixture class for setting up spdlog.

#pragma once

#include <gtest/gtest.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace platform::tests {

class BaseTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set up the logger to write to the terminal
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("console", console_sink));
        spdlog::set_level(spdlog::level::debug); // Set the log level to info
        spdlog::flush_on(spdlog::level::debug);  // Flush on info level
    }
};
}