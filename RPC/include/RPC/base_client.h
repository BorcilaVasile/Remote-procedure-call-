#include <unistd.h>
#include <RPC/client_socket.h>
#include <RPC/procedure_format.pb.h>


class BaseClient{
protected:
    ClientSocket* client_socket;

public: 
    void connectToServer(std::string ip="0.0.0.0",uint16_t port=8080);

protected: 
    BaseClient();

    void sendData(char* message, int length);
    void sendData(RPC::Request& request);
    void receiveData(char* message, int length);
    RPC::Response receiveResponse();

    template <typename ReturnType, typename... Args>
    ReturnType callRemoteFunction(const std::string& function_name, Args... args){
        RPC::Request request;
        request.set_function_name(function_name); 

        (void)std::initializer_list<int>{(addArgument(request, args), 0)...};


        sendData(request);
        RPC::Response response=receiveResponse();
        if (response.return_value().status() == RPC::Status::OK) {
            return extractResult<ReturnType>(response.return_value());
        } else {
            fail("Error calling the requested function");
        }
    }

private: 
    template <typename T>
    void addArgument(RPC::Request& request, T arg){
        RPC::Argument* new_arg = request.add_args();
        if constexpr (std::is_same<T, int>::value) {
            new_arg->set_int_val(arg);
        } else if constexpr (std::is_same<T, double>::value) {
            new_arg->set_double_val(arg);
        } else if constexpr (std::is_same<T, std::string>::value) {
            new_arg->set_string_val(arg);
        } else if constexpr (std::is_same<T, bool>::value) {
            new_arg->set_bool_val(arg);
        }
    }

    template <typename T> 
    T extractResult(const RPC::ReturnValue& return_value){
                if constexpr (std::is_same<T, int>::value) {
            return return_value.int_result();
        } else if constexpr (std::is_same<T, double>::value) {
            return return_value.double_result();
        } else if constexpr (std::is_same<T, std::string>::value) {
            return return_value.string_result();
        } else if constexpr (std::is_same<T, bool>::value) {
            return return_value.bool_result();
        } else {
            throw std::runtime_error("Unsupported return type");
        }
    }

};