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

    RPC::Argument arg;
    arg.set_string_val("My dear friend Jasmine");

    RPC::Response response = client->callFunction("sayHello", {arg});

    // Verificăm răspunsul
    if (response.return_value().status() == RPC::Status::OK) {
        std::cout << "Response from server: " << response.return_value().string_result() << std::endl;
    } else {
        std::cout << "Error: " << response.return_value().message() << std::endl;
    }

    return 0;
    delete client;
}