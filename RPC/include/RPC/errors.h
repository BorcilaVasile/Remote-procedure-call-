#ifndef ERRORS_H
#define ERRORS_H


#include <iostream> 
#include <string> 
#include <stdexcept>
#include <RPC/procedure_format.pb.h>


class ErrorHandler{
private: 
    std::unordered_map<RPC::Status, std::string> error_messages;
public: 
    ErrorHandler();


    //handle errors based on status
    void handle(RPC::Status status, const std::string& custom_message="");

    //retrieve error message for a status
    std::string getErrorMessage(RPC::Status status) const;
    
};


class RPCException: public std::runtime_error{
public: 
    explicit RPCException(const std::string& message): std::runtime_error(message){};
};
#endif