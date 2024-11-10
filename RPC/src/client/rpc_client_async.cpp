#include <RPC/rpc_client.h>

std::future<std::string> Client::sayHelloAsync(std::string name){
    return callRemoteFunctionAsync<std::string>("sayHello",name);
}

std::future<std::string> Client::disconnectAsync(){
    return callRemoteFunctionAsync<std::string>("disconnect");
}

std::future<void> Client::nonExistentFunctionAsync(){
    return callRemoteFunctionAsync<void>("nonExistentFunction");
}