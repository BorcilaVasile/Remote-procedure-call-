#include <RPC/socket_connection.h>


class ClientSocket : public Socket{ 
protected: 
    struct sockaddr_in address_dest; 
    struct sockaddr_in address_source; 

public:
    ClientSocket();
    void setClientAddress(std::string ip);
    void connectToServer(std::string ip, uint16_t port);
};