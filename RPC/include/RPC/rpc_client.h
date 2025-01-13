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
    std::string returnTypeName(void* variable);
    int** multiplyMatrix(int** A, int** B, int dimension);
    int open(const char* filename, int flags, int mode);
    int open(const char* filename, int flags);
    ssize_t read(int fd, char *buf, int count);
    ssize_t read(int fd, int* buf, int count);
    ssize_t write(int fd,const char* buf, int count);
    int close(int fd);

    
    std::string disconnect();

    //asynchronous functions
    std::future<std::string> sayHelloAsync(std::string name);
    std::future<void> nonExistentFunctionAsync();
    std::future<std::string> disconnectAsync(); 
    ~Client()=default;
};