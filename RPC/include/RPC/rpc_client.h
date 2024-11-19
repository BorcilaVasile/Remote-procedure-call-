#include <RPC/functions.h>
#include <RPC/base_client.h>

class Client: public BaseClient, public RemoteFunction{
public: 
    Client(bool useTLS=false): BaseClient(){
        this->useTLS=useTLS;
    };

    //synchronous functions
    std::string sayHello(std::string name);
    void nonExistentFunction();
    void authenticate(std::string username, std::string password);
    std::string disconnect();

    //asynchronous functions
    std::future<std::string> sayHelloAsync(std::string name);
    std::future<void> nonExistentFunctionAsync();
    std::future<std::string> disconnectAsync(); 
    ~Client()=default;
};