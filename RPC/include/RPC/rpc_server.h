#include <unistd.h>
#include <RPC/socket_connection.h>

class Server{
private: 
    Socket* server_socket;

public: 
    Server();
    void acceptConnectionsOnServer();
    void receiveFunction(); 
    void sendResult();
    ~Server(){
        free(server_socket);
    }
};