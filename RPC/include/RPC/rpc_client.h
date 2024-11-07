#include <unistd.h>
#include <RPC/client_socket.h>
#include <RPC/procedure_format.pb.h>
#include <RPC/functions.h>

class Client: public RemoteFunction{
private: 
    ClientSocket* client_socket;

public: 

    Client();
    void connectToServer(std::string ip="0.0.0.0",uint16_t port=8080);

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