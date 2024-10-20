#include <RPC/rpc_client.h>

Client::Client(){
    client_socket=new Socket(socketType::CLIENT);
    printf("\nThe client is ready to connect to the server\n"); 
}

void Client::connectToServer(){
    client_socket->connectToServer();
    printf("The client connected succesfully to the server\n");
}

void Client::sendData(char* message, int length){
    if(client_socket->sendData(message,length)==-1)
        fail("Failed to send data to server");
}

void Client::receiveData(char* message, int length){
    if(client_socket->receiveData(message, length)==-1)
        fail("Failed to receive data from the server");
}