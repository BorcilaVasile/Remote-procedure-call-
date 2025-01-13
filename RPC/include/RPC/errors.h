#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <RPC/procedure_format.pb.h>

class ErrorHandler {
private:
    std::unordered_map<RPC::Status, std::string> error_messages;

    // Private method to log errors

public:
    ErrorHandler();

    // Handle errors based on status
    void handle(RPC::Status status, const std::string& custom_message = "", bool log_error = true);

    // Retrieve error message for a status
    std::string getErrorMessage(RPC::Status status) const;

    // Add custom error message
    void addCustomErrorMessage(RPC::Status status, const std::string& message);
    void logError(const std::string& error_message) const;
};

class RPCException : public std::runtime_error {
public:
    explicit RPCException(const std::string& message) : std::runtime_error(message) {}
};

#endif
