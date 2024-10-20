#include <unistd.h>
#include <RPC/socket_connection.h>

class Server{
private: 
    Socket* server_socket;
    Socket* client_socket; 

public: 
    Server();
    void acceptConnectionsOnServer();
    void receiveFunction(); 
    void sendResult();
    void receiveMessage(char* message, int length);
    void sendMessage(char* message,int length);
    ~Server(){
        delete server_socket;
        if(client_socket!=nullptr)
            delete client_socket;
    }
};