#include <RPC/rpc_server.h>

Server::Server(){
    server_socket=new Socket(socketType::SERVER);
    server_socket->bindSocket();
    server_socket->listenForConnections();
    printf("\nSocket on the server is listening for connections\n"); 
}

void Server::acceptConnectionsOnServer(){
    Socket* client_socket = server_socket->acceptConnections();
    if (client_socket == nullptr) {
        fail("Failed to accept connection from client");
        return;
    }

    printf("Client connected\n");
    this->client_socket = client_socket;
}

void Server::receiveMessage(char* message, int length){
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }
    if(client_socket->receiveData(message, length)==-1)
    {
        printf("%s\n", message);
        fail("Failed to receive message from the client");
    }
}   

void Server::sendMessage(char* message, int length){
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }
    if(client_socket->sendData(message, length)==-1)
        fail("Failed to send message to the client");
}