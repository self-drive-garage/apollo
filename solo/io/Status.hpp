#pragma once

#include <cstring>
#include <iostream>
#include <optional>
#include <string>

namespace solo::platform::io {

/// @enum STATUS
/// @brief Represents the basic status of an operation.
///
/// Enumerates the possible high-level outcomes of an operation,
/// distinguishing between successful completion and errors.
enum class STATUS
{
    SUCCESS, ///< Operation succeeded without any error.
    ERROR,    ///< Operation failed, specifics provided by ERROR enum.
    
};

/// @enum ERROR
/// @brief Defines specific error types for operations that result in errors.
///
/// Enumerates detailed error types that describe the nature of the failure
/// when the status is ERROR.
enum class ERROR
{
    NONE,                ///< No error occurred.
    TIMEOUT,             ///< Operation timed out.
    COMMUNICATION_ERROR, ///< Communication failure during operation.
    SENSOR_FAILURE,      ///< Sensor malfunction or failure detected.
    ERROR_CREATING_DEVICE,    ///< Error creating device.
    ERROR_OPENING_DEVICE,     ///< Error opening device.
    ALREADY_CLOSED,           ///< Device is already closed.
    INVALID_PARAMETER,        ///< Invalid parameter.
    ERROR_SETTING_SOCKET_ISO_TP_OPTIONS, ///< Error setting socket ISO-TP options.
    ALREADY_OPEN,                       ///< Device is already open.
    SENSOR_NOT_FOUND,                   ///< Sensor not found.
    SENSOR_NOT_INITIALIZED,            ///< Sensor not initialized.
    SENSOR_NOT_OPENED,                 ///< Sensor not opened.
    INVALID_DATA,                       ///< Invalid data.
};

/// @class Status
/// @brief Represents the outcome of an operation, with detailed error information if applicable.
///
/// This class encapsulates both a basic status (success or error) and,
/// in the case of errors, provides detailed information about the type of error.
class Status
{
public:
    /// @brief Constructor for Status.
    /// @param bs The basic status (SUCCESS or ERROR).
    /// @param et The specific error type if an error has occurred.
    /// @param errorMessage A detailed error message if an error has occurred.
    /// @param errNum An optional errno value if an error has occurred.
    Status(STATUS bs = STATUS::SUCCESS,
           ERROR et = ERROR::NONE,
           std::string errorMessage = std::string(),
           std::optional<int> errNum = std::nullopt)
        : basicStatus_{bs}
        , errorType_{et}
        , errorMessage_{std::move(errorMessage)}
        , errNum_{errNum}
    {}

    /// @brief Checks if the status indicates success.
    /// @return True if the status is SUCCESS, false otherwise.
    bool isSuccess() const { return basicStatus_ == STATUS::SUCCESS; }

    /// @brief Checks if the status indicates an error.
    /// @return True if the status is ERROR, false otherwise.
    bool isError() const { return basicStatus_ == STATUS::ERROR; }

    /// @brief Checks if the error type matches the specified type.
    /// @param et The error type to compare against.
    /// @return True if the error type matches, false otherwise.
    bool isErrorType(ERROR et) const { return errorType_ == et; }

    /// @brief Converts the status to a string representation.
    /// @return A string that describes the status or error in detail.
    std::string toString() const
    {
        std::string result;
        if (isSuccess()) {
            result = "Success";
        } else {
            switch (errorType_) {
                case ERROR::TIMEOUT:
                    result = "Timeout Error: ";
                    break;
                case ERROR::COMMUNICATION_ERROR:
                    result = "Communication Error: ";
                    break;
                case ERROR::SENSOR_FAILURE:
                    result = "Sensor Failure: ";
                    break;
                case ERROR::ERROR_CREATING_DEVICE:
                    result = "Error Creating Device: ";
                    break;
                case ERROR::ERROR_OPENING_DEVICE:
                    result = "Error Opening Device: ";
                    break;
                case ERROR::INVALID_PARAMETER:
                    result = "Invalid Parameter: ";
                    break;
                case ERROR::ERROR_SETTING_SOCKET_ISO_TP_OPTIONS:
                    result = "Error Setting SOCKET ISO-TP Options: ";
                    break;
                default:
                    result = "Unknown Error: ";
                    break;
            }
            result += errorMessage_;
            if (errNum_) {
                result += " (errno: " + std::to_string(*errNum_) + " - " + std::strerror(*errNum_) + ")";
            }
        }
        return result;
    }

    /// @brief Overloads the output stream operator for Status objects.
    /// @param os The output stream to send the data.
    /// @param status The Status object to output.
    /// @return A reference to the modified output stream.
    /// @details Utilizes the toString method to output the status.
    friend std::ostream& operator<<(std::ostream& os, const Status& status)
    {
        os << status.toString();
        return os;
    }

private:
    STATUS basicStatus_;
    ERROR errorType_;
    std::string errorMessage_;
    std::optional<int> errNum_;
};

} // namespace platform::io
