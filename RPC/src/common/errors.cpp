#include <RPC/errors.h>

ErrorHandler::ErrorHandler(){
    error_messages[RPC::Status::OK]="Operation succesfull";
    error_messages[RPC::Status::ERROR]="General error has occured";
    error_messages[RPC::Status::INVALID_ARGUMENT]="Invalid argument passed to procedure";
    error_messages[RPC::Status::NOT_FOUND]="Called procedure was not found";
    error_messages[RPC::Status::PERMISSION_DENIED]="Permission deniend";
    error_messages[RPC::Status::INTERNAL_ERROR]="Internal server error";
}

void ErrorHandler::handle(RPC::Status status, const std::string& custom_message)
{
    if(status!=RPC::Status::OK)
    {
        std::string error_message=getErrorMessage(status);
        if(!custom_message.empty())
            error_message+="Details: "+ custom_message;
        throw RPCException(error_message);
    }
}

std::string ErrorHandler::getErrorMessage(RPC::Status status) const{
    auto it=error_messages.find(status);
    if(it!=error_messages.end())
        return it->second;
    else
        return "Unknown error";
}