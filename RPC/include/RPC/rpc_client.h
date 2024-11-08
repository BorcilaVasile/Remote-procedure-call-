#include <RPC/functions.h>
#include <RPC/base_client.h>

class Client: public BaseClient, public RemoteFunction{
public: 
    Client(): BaseClient(){};

    std::string sayHello(std::string name);
    std::string disconnect();
    ~Client()=default;
};