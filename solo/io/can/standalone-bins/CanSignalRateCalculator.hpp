
#pragma once

/// Calculates the instantaneous rate of a CAN signal by using the counter
/// variable and the known frequency of the CAN message that contains it.
class CanSignalRateCalculator
{
public:
    explicit CanSignalRateCalculator(const double frequency)
        : frequency(frequency)
    {}

    double update(const double current_value, const int current_counter)
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
    int previous_counter{0};
    double previous_value{0};
    double rate{0};
};
