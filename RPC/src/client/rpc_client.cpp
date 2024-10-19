#include <RPC/rpc_client.h>

Client::Client(){
    client_socket=new Socket(socketType::CLIENT);
    printf("\nThe client is ready to connect to the server\n"); 
}

void Client::connectToServer(){
    client_socket->connectToServer();
    printf("The client connected succesfully to the server\n");
}