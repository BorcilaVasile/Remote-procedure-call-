#include <RPC/functions.h>
#include <RPC/base_client.h>

class Client: public BaseClient, public RemoteFunction{
public: 
    Client(): BaseClient(){};

    //synchronous functions
    std::string sayHello(std::string name);
    void nonExistentFunction();
    std::string disconnect();

    //asynchronous functions
    std::future<std::string> sayHelloAsync(std::string name);
    std::future<void> nonExistentFunctionAsync();
    std::future<std::string> disconnectAsync(); 
    ~Client()=default;
};