#include <unistd.h>
#include <RPC/socket_connection.h>
#include <RPC/procedure_format.pb.h>

class Client{
private: 
    Socket* client_socket;

public: 
    Client();
    void connectToServer();
    RPC::Response callFunction(std::string function_name, std::vector<RPC::Argument> args);
    void sendData(char* message, int length);
    void sendData(RPC::Request& request);
    void receiveData(char* message, int length);
    RPC::Response receiveResponse();
    ~Client(){
        delete client_socket;
    }
};