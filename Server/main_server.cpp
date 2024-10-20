#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <string.h> 
#include <RPC/rpc_server.h>


int main() {
    Server* server = new Server();
    server->acceptConnectionsOnServer();

    char message[1024] = {0}; 
    server->receiveMessage(message, sizeof(message) - 1);
    printf("Received from client: %s\n", message);

    char response[1024];
    strcpy(response, "Hello from the server");
    server->sendMessage(response, strlen(response));

    // Eliberare memorie și închidere server
    delete server;
    return 0;
}
