#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <RPC/rpc_client.h>

int main(){
    Client* client = new Client();
    client->connectToServer("0.0.0.0", 8080);
    bool authenticated = client->authenticate("username", "password");
    if(authenticated) {
        fprintf(stdout, "\nClient connected to server\n");
        std::future<std::string> sayHelloFuture = client->sayHelloAsync("Jasmine");

        std::string something;
        std::cin >> something;
        
        std::future<std::string> disconnectFuture = client->disconnectAsync();
        
        std::cout << "sayHelloAsync result: " << sayHelloFuture.get() << std::endl;
        std::cout << "disconnectAsync result: " << disconnectFuture.get() << std::endl;
    } else {
        std::cerr << "Failed to authenticate to server." << std::endl;
    }

    delete client;
    return 0;
}
