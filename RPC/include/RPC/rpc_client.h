#include <unistd.h>
#include <RPC/socket_connection.h>
#include <RPC/procedure_format.pb.h>
#include <RPC/functions.h>

class Client: public RemoteFunction{
private: 
    Socket* client_socket;

public: 

    Client();
    void connectToServer();

    void sendData(char* message, int length);
    void sendData(RPC::Request& request);
    void receiveData(char* message, int length);
    RPC::Response receiveResponse();

    RPC::Response callFunction(std::string function_name, std::vector<RPC::Argument> args);

    std::string sayHello(std::string name);
    std::string disconnect();
    ~Client(){
        delete client_socket;
    }
};