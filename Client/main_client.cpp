#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <RPC/rpc_client.h>

int main(){
    Client* client=new Client;
    client->connectToServer();
    //apel catre functia sayHello 
    std::cout<<client->sayHello("Jasmine");

    sleep(10);
    std::cout<<client->disconnect();
    delete client;
    return 0;
}