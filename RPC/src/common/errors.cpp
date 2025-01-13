#include <RPC/errors.h>
#include <fstream>

ErrorHandler::ErrorHandler() {
    error_messages[RPC::Status::OK] = "Operation successful";
    error_messages[RPC::Status::ERROR] = "A general error has occurred";
    error_messages[RPC::Status::INVALID_ARGUMENT] = "Invalid argument passed to procedure";
    error_messages[RPC::Status::NOT_FOUND] = "The called procedure was not found";
    error_messages[RPC::Status::PERMISSION_DENIED] = "Permission denied";
    error_messages[RPC::Status::INTERNAL_ERROR] = "Internal server error";
}

void ErrorHandler::handle(RPC::Status status, const std::string& custom_message, bool log_error) {
    if (status != RPC::Status::OK) {
        std::string error_message = getErrorMessage(status);
        if (!custom_message.empty()) {
            error_message +=": "+ custom_message;
        }

        if (log_error) {
            logError(error_message);
        }

        throw RPCException(error_message);
    }
}

void ErrorHandler::addCustomErrorMessage(RPC::Status status, const std::string& message) {
    error_messages[status] = message;
}

std::string ErrorHandler::getErrorMessage(RPC::Status status) const {
    auto it = error_messages.find(status);
    if (it != error_messages.end()) {
        return it->second;
    } else {
        return "Unknown error";
    }
}

void ErrorHandler::logError(const std::string& error_message) const {
    // Log to a file
    std::ofstream log_file("error_log.txt", std::ios::app);
    if (log_file.is_open()) {
        log_file << error_message << std::endl;
    }
     log_file.close();
    std::cerr << "[Error] " << error_message << std::endl;
}
