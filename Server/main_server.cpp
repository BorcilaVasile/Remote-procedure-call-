#include <stdio.h>
#include <stdlib.h> 

#include <RPC/rpc_server.h>

int main() {
    Server* server = new Server(10,"0.0.0.0", 8000);
    server->loadClients("Clients.txt");

    printf("Starting server...\n");
    server->start();
    
    delete server;
    printf("Server stopped.\n");
    return 0;
}
