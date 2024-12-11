#include <exception>
#include <string>
#include <utility>

namespace platform::io::can::exceptions {

/// @brief Base class for all CAN-related exceptions.
class CANException : public std::exception
{
public:
    /// @brief Constructor for CANException.
    /// @param message The error message.
    /// @param socket The socket descriptor related to the error.
    CANException(std::string message, int32_t socket = -1)
        : message_(std::move(message))
        , socket_(socket)
    {}

    /// @brief Returns the error message.
    /// @return The error message.
    const char* what() const noexcept override { return message_.c_str(); }

    /// @brief Returns the socket descriptor related to the error.
    /// @return The socket descriptor.
    int32_t getSocket() const noexcept { return socket_; }

private:
    std::string message_;
    int32_t socket_;
};

/// @brief An exception that may be thrown when an error occurs while closing a CAN socket.
class CANCloseException : public CANException
{
public:
    /// @brief Constructor for CANCloseException.
    /// @param message The error message.
    /// @param socket The socket descriptor related to the error.
    CANCloseException(std::string message, int32_t socket)
        : CANException(std::move(message), socket)
    {}
};

/// @brief An exception that may be thrown when an error occurs while initializing a CAN socket.
class CANInitException : public CANException
{
public:
    /// @brief Constructor for CANInitException.
    /// @param message The error message.
    CANInitException(std::string message)
        : CANException(std::move(message))
    {}
};


} // namespace platform::io::can::exceptions
