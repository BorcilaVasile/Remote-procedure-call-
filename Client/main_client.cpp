#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <RPC/errors.h>

int main(){

    int socket_fd;
    socket_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd==-1)
        fail("Error at socket creation for client");
    struct sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(8080);

    if(connect(socket_fd, (struct sockaddr *)&address, sizeof(address))==-1)
    {
        close(socket_fd);
        fail("Error at connecting with the server");
    }


    char buffer[255]="Hello from client!";
    send(socket_fd, buffer, strlen(buffer), 0); 
    while(strcmp(buffer,"exit\n")!=0)
    {
        fgets(buffer, sizeof(buffer),stdin);
        send(socket_fd, buffer, strlen(buffer), 0); 
    }


    char response[255];
    int bytes_received=recv(socket_fd, response, sizeof(response)-1, 0);
    if(bytes_received==-1){
        close(socket_fd);
        fail("Error at receiving data from the client");
    }
    
    response[bytes_received]='\0';
    printf("Server: %s", response); 
    
    close(socket_fd);
    return 0; 
}