#include <unistd.h>
#include <RPC/socket_connection.h>

class Server{
private: 
    Socket server_socket;

public: 
    Server();
    void receiveFunction(); 
    void sendResult();
};