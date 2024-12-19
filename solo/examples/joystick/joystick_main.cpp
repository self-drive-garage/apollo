#include <csignal>
#include <atomic>
#include "cyber/cyber.h"
#include "cyber/init.h"
#include "cyber/node/reader.h"
#include "cyber/node/writer.h"
#include "modules/common_msgs/control_msgs/control_cmd.pb.h"
#include "solo/modules/control/AckermanRobotKit.hpp"
#include "solo/modules/joystick/Joystick.hpp"

using namespace solo::modules;
using namespace solo::drivers;
using namespace apollo::cyber;
using apollo::control::ControlCommand;

// Atomic flag to control the program's running state
std::atomic<bool> keepRunning(true);

// Signal handler for Ctrl-C
void signalHandler(int signum) {
    keepRunning = false; // Set the atomic flag to false
    AWARN << "\nCtrl-C pressed. Exiting gracefully...\n";
}

int main() {
    // Register the signal handler
    std::signal(SIGINT, signalHandler);

    auto joystick = joystick::Joystick();
    joystick.Init();

    auto robot = control::AckermanRobotKit();

    std::shared_ptr<ControlCommand> command_message(new(std::nothrow)
                                                        ControlCommand);



    // Main loop
    while (keepRunning) {

        auto joystickControl = joystick.update();

        command_message->set_acceleration(joystickControl.throttle_);
        command_message->set_steering_rate(joystickControl.steering_angle_);
        command_message->set_throttle(joystickControl.throttle_);
        command_message->set_brake(joystickControl.brake_);
        command_message->set_parking_brake(joystickControl.parking_brake_);

        robot.execute(command_message);

    }

    // Clean up resources before exiting
    AWARN << "Cleaning up resources...\n";

    return 0;
}
