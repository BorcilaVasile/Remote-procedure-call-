#include <RPC/socket_connection.h>

class ServerSocket: public Socket{ 
public: 
    ServerSocket(std::string ip, uint16_t port);
    void bindSocket();
    void listenForConnections();
    Socket* acceptConnections();
};
