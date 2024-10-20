#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <RPC/rpc_client.h>

int main(){

    Client* client=new Client();
    client->connectToServer();

    char message[1024];
    strcpy(message,"Hello from the client\0");
    client->sendData(message,strlen(message));

    char response[1024]={0};
    client->receiveData(response, sizeof(response)-1);
    printf("Received from the server: %s", response);
    delete client;
    return 0; 
}