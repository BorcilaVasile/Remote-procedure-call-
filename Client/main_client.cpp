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
    free(client);
    // if(connect(socket_fd, (struct sockaddr *)&address, sizeof(address))==-1)
    // {
    //     close(socket_fd);
    //     fail("Error at connecting with the server");
    // }


    // char buffer[255]="Hello from client!";
    // send(socket_fd, buffer, strlen(buffer), 0); 
    // while(strcmp(buffer,"exit\n")!=0)
    // {
    //     fgets(buffer, sizeof(buffer),stdin);
    //     send(socket_fd, buffer, strlen(buffer), 0); 
    // }


    // char response[255];
    // int bytes_received=recv(socket_fd, response, sizeof(response)-1, 0);
    // if(bytes_received==-1){
    //     close(socket_fd);
    //     fail("Error at receiving data from the client");
    // }
    
    // response[bytes_received]='\0';
    // printf("Server: %s", response); 
    
    // close(socket_fd);
    return 0; 
}