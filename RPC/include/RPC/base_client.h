#include <unistd.h>
#include <future>
#include <RPC/encryption.h>
#include <RPC/client_socket.h>
#include <RPC/procedure_format.pb.h>
#include <RPC/errors.h>

class BaseClient {
protected:
    ClientSocket* client_socket;
    ErrorHandler errorHandler;
    std::string token;
    int client_id;
    bool useTLS=true; 

private: 
    SSL_CTX* ctx; 
    SSL* ssl; 

public: 
    // connect synchronous to server
    void connectToServer(std::string ip="0.0.0.0",uint16_t port=8080);
    // connect asynchronous to server
    std::future<void> connectToServerAsync(std::string ip="0.0.0.0", uint16_t port=8080);
protected: 
    BaseClient();
    ~BaseClient();
    void authenticateUser(std::string username, std::string password, int uid, int gid);

    //synchronous calls
    void sendData(char* message, int length);
    void receiveData(char* message, int* length);

    void sendRequest(RPC::Request& request);
    RPC::Response receiveResponse();

    template <typename ReturnType, typename... Args>
    ReturnType callRemoteFunction(const std::string& function_name, Args... args) {
        try{
            RPC::FunctionRequest function_request;
            function_request.set_function_name(function_name); 
            function_request.set_token(this->token);
            function_request.set_client_id(client_id);

            (void)std::initializer_list<int>{(addArgument(function_request, args), 0)...};

            std::cout << "Sending request for function: " << function_name << std::endl;

            RPC::Request request;
            *request.mutable_function_request() = function_request;

            this->sendRequest(request);

            RPC::Response response = receiveResponse();
            errorHandler.handle(response.return_value().status(), response.return_value().message());
            return extractResult<ReturnType>(response.return_value());
        }catch(const RPCException& e){
            std::cerr<<"RPC error:"<<e.what()<<std::endl;
        }catch(const std::exception& e){
            std::cerr<<"Unexpected error: "<<e.what()<<std::endl;
        }
        return ReturnType();
    }

    //asynchronous calls
    std::future<void> sendDataAsync(char* message, int length); 
    std::future<void> receiveDataAsync(char* message, int* length); 

    std::future<void> sendRequestAsync(RPC::Request& request); 
    std::future<RPC::Response> receiveResponseAsync();

    template<typename ReturnType, typename... Args>
    std::future<ReturnType> callRemoteFunctionAsync(const std::string& function_name, Args... args) { 
        return std::async(std::launch::async, [this, function_name, args...]()-> ReturnType {
            try{
                RPC::FunctionRequest function_request;
                function_request.set_function_name(function_name); 
                function_request.set_token(this->token);
                function_request.set_client_id(client_id);


                (void)std::initializer_list<int>{(addArgument(function_request, args), 0)...};

                std::cout << "Sending request for function: " << function_name << std::endl;

                RPC::Request request;
                *request.mutable_function_request() = function_request;

                this->sendRequestAsync(request);
                
                RPC::Response response = receiveResponse();
                errorHandler.handle(response.return_value().status(), response.return_value().message());
                return extractResult<ReturnType>(response.return_value());
            }catch(const RPCException& e)
            {
                std::cerr<<"RPC error:"<<e.what()<<std::endl;
            }catch(const std::exception& e){
                std::cerr<<"Unexpected error: "<<e.what()<<std::endl;
            }
            return ReturnType();
        });
}


private: 
    template <typename T>
    void addArgument(RPC::FunctionRequest& function_request, T arg) {
        RPC::Argument* new_arg = function_request.add_args();
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
    T extractResult(const RPC::ReturnValue& return_value) {
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

private: 
    SSL_CTX* createContext();
    void configureContext(SSL_CTX* ctx);
};