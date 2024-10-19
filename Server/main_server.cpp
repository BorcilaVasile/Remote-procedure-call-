#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <string.h> 
#include <RPC/errors.h>


int main(){

    int socket_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd==-1)
        fail("Error at socket creation for server");

    struct sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(8080);
    if(bind(socket_fd,(struct sockaddr*)&address, sizeof(address))==-1)
        fail("Error at binding the socket on the server");

    if(listen(socket_fd, SOMAXCONN)==-1)
    {
        close(socket_fd);
        fail("Error at listening on the server socket");
    }

    printf("Server is listening on port 8080\n");

    socklen_t address_len=sizeof(address);
    int client_fd=accept(socket_fd,(struct sockaddr*)&address,&address_len);
    if(client_fd==-1)
    {
        close(client_fd);
        fail("Error at accepting connections on the socket server");
    }

    printf("Client connected\n");
   
    char buffer[255]; 
    ssize_t bytes_received=recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if(bytes_received==-1){
        close(client_fd);
        close(socket_fd);
        fail("Error at receiving data from the client");
    }
    
    buffer[bytes_received]='\0';
    printf("Client: %s", buffer); 

    while(strcmp(buffer,"exit\n")!=0){
        bytes_received=recv(client_fd, buffer, sizeof(buffer)-1, 0);
        if(bytes_received==-1){
             close(client_fd);
            close(socket_fd);
            fail("Error at receiving data from the client");
        }
        buffer[bytes_received]='\0';
        printf("Client: %s", buffer); 
    }

    // const char *response="Hello from server!"; 
    // send(client_fd, response, strlen(response),0);

    close(client_fd); 
    close(socket_fd);
    return 0;
}