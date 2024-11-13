#include <RPC/client_socket.h>

ClientSocket::ClientSocket(): Socket(socketType::CLIENT){}

void ClientSocket::setClientAddress(std::string ip)
{

    address_source.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip.c_str(), &address_source.sin_addr.s_addr)<=0) 
        fail("Invalid IP address");
}
bool ClientSocket::connectToServer(std::string ip, uint16_t port){
    address_dest.sin_family = AF_INET;
    address_dest.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &address_dest.sin_addr.s_addr) <= 0) 
    {
        fail("Invalid IP address for server");
        return false; 
    }

    if(connect(file_descriptor, (struct sockaddr *)&address_dest, sizeof(address_dest))==-1)
    {
        close(file_descriptor);
        fail("Error at connecting with the server");
        return false;
    }
    return true; 
}