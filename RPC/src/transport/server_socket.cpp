#include <RPC/server_socket.h>

ServerSocket::ServerSocket(std::string ip, uint16_t port): Socket(socketType::SERVER,ip,port){}

void ServerSocket::bindSocket()
{
    if(bind(file_descriptor,(struct sockaddr*)&address, sizeof(address))==-1)
        fail("Error at binding the socket on the server");
}

void ServerSocket::listenForConnections()
{
     if (listen(file_descriptor, SOMAXCONN) == -1) {
        close(file_descriptor);
        fail("Error at listening on the server socket");
    }
}

Socket *ServerSocket::acceptConnections()
{
    socklen_t address_len = sizeof(Socket::address);
    int client_fd = accept(file_descriptor, (struct sockaddr*)&address, &address_len);
    if (client_fd == -1) {
        fail("Error at accepting connections on the socket server");
        return nullptr;
    }

    Socket* clientSocket = new Socket(socketType::CLIENT);
    clientSocket->file_descriptor = client_fd;
    clientSocket->setAddress(address);
    return clientSocket;
}
