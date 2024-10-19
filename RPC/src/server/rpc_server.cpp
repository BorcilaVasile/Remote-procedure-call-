#include <RPC/rpc_server.h>

Server::Server(){
    server_socket=new Socket(socketType::SERVER);
    server_socket->bindSocket();
    server_socket->listenForConnections();
    printf("\nSocket on the server is listening for connections\n"); 
}

void Server::acceptConnectionsOnServer(){
    server_socket->acceptConnections();
}