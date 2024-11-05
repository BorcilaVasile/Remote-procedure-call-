#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <string.h> 
#include <RPC/rpc_server.h>
#include <RPC/procedure_format.pb.h>

int main() {
    Server* server = new Server();

    server->acceptConnectionsOnServer();
    server->start();
    
    delete server;
    return 0;
}
