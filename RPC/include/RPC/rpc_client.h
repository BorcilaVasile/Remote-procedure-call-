#include <unistd.h>
#include <RPC/socket_connection.h>

class Client{
private: 
    Socket* client_socket;

public: 
    Client();
    void connectToServer();
    void sendData(char* message, int length);
    void receiveData(char* message, int length);
    ~Client(){
        delete client_socket;
    }
};