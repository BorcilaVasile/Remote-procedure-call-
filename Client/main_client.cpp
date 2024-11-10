#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <RPC/rpc_client.h>

int main(){
    Client* client = new Client();
    std::future<void> connectFuture = client->connectToServerAsync("0.0.0.0", 8080);
    
    connectFuture.get();
    
    std::future<std::string> sayHelloFuture = client->sayHelloAsync("Jasmine");
    
    std::future<std::string> disconnectFuture = client->disconnectAsync();
    
    std::cout << "sayHelloAsync result: " << sayHelloFuture.get() << std::endl;
    std::cout << "disconnectAsync result: " << disconnectFuture.get() << std::endl;

    delete client;

    return 0;
}
