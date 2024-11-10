#include <RPC/rpc_client.h>

std::string Client::sayHello(std::string name){
    return callRemoteFunction<std::string>("sayHello", name);
}

std::string Client::disconnect(){
    return callRemoteFunction<std::string>("disconnect");
}

void Client::nonExistentFunction(){
    return callRemoteFunction<void>("nonExistentFunction");
}
