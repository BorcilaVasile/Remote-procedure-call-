#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <string.h> 
#include <RPC/rpc_server.h>


int main(){
   
    Server* server=new Server();
    server->acceptConnectionsOnServer();
    free(server);
    // char buffer[255]; 
    // ssize_t bytes_received=recv(client_fd, buffer, sizeof(buffer)-1, 0);
    // if(bytes_received==-1){
    //     close(client_fd);
    //     close(socket_fd);
    //     fail("Error at receiving data from the client");
    // }
    
    // buffer[bytes_received]='\0';
    // printf("Client: %s", buffer); 

    // while(strcmp(buffer,"exit\n")!=0){
    //     bytes_received=recv(client_fd, buffer, sizeof(buffer)-1, 0);
    //     if(bytes_received==-1){
    //          close(client_fd);
    //         close(socket_fd);
    //         fail("Error at receiving data from the client");
    //     }
    //     buffer[bytes_received]='\0';
    //     printf("Client: %s", buffer); 
    // }

    // // const char *response="Hello from server!"; 
    // // send(client_fd, response, strlen(response),0);

    // close(client_fd); 
    // close(socket_fd);
    return 0;
}