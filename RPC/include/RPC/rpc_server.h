#include <unistd.h>
#include <RPC/socket_connection.h>
#include <RPC/procedure_format.pb.h>


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
    RPC::Response processRequest(RPC::Request& request);
    void sendMessage(char* message,int length);
    void sendResult(RPC::Response& response);
    void closeConnection();
    ~Server(){
        delete server_socket;
        if(client_socket!=nullptr)
            delete client_socket;
    }
};