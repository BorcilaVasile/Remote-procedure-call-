#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <RPC/rpc_client.h>

int main(){
    Client* client = new Client(true);
    client->connectToServer("0.0.0.0", 8000);
    client->authenticate("username1", "password1");

    std::string sayHelloFuture = client->sayHello("Jasmine");
    std::cout << "sayHelloAsync result: " << sayHelloFuture << std::endl;
    client->nonExistentFunction();
    std::string disconnectFuture = client->disconnect();
    std::cout << "disconnect result: " << disconnectFuture<< std::endl;
        

    delete client;
    return 0;
}


