#include <RPC/client_socket.h>

ClientSocket::ClientSocket(): Socket(socketType::CLIENT){}

void ClientSocket::setClientAddress(std::string ip)
{   
    address_source.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip.c_str(), &address_source.sin_addr.s_addr)<=0) 
        throw RPCException("Invalid IP address for the client side");
}
void ClientSocket::connectToServer(std::string ip, uint16_t port){
    address_dest.sin_family = AF_INET;
    address_dest.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &address_dest.sin_addr.s_addr) <= 0) 
        throw RPCException("Invalid IP address");

    if(connect(file_descriptor, (struct sockaddr *)&address_dest, sizeof(address_dest))==-1)
    {
        close(file_descriptor);
        throw RPCException("Failed to connect to the server");
    }
}