#include <RPC/rpc_client.h>
#include <iostream>
#include <unistd.h> // pentru getuid() și getgid()

std::string Client::sayHello(std::string name){
    return callRemoteFunction<std::string>("sayHello", name);
}

std::string Client::disconnect(){
    return callRemoteFunction<std::string>("disconnect");
}

void Client::nonExistentFunction(){
    return callRemoteFunction<void>("nonExistentFunction");
}

bool Client::authenticate(std::string username, std::string password)
{
    int uid = getuid();
    int gid = getgid();
    std::cout << "Sending authentication request for user: " << username << " with UID: " << uid << " and GID: " << gid << std::endl;
    return authenticateUser(username, password, uid, gid);
}
