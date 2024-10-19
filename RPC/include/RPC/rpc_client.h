#include <unistd.h>
#include <RPC/socket_connection.h>

class Client{
private: 
    Socket* client_socket;

public: 
    Client();
    void connectToServer();
    ~Client(){
        free(client_socket);
    }
};