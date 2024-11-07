#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <fcntl.h>
#include <cerrno> 
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <RPC/errors.h>

enum class socketType{
    SERVER, CLIENT
};
class Socket{ 
protected: 
    socketType type;
    struct sockaddr_in address;

public: 
    int file_descriptor;

private: 
    void setUpServerSocket(std::string ip="0.0.0.0",uint16_t port=8080);
    void setUpClientSocket(std::string ip="127.0.0.1", uint16_t port=8080);
    
protected: 

public:
    Socket(socketType type,std::string ip="127.0.0.1", uint16_t port=8080);
    void setAddress(std::string ip, uint16_t port);
    void setAddress(struct sockaddr_in address);

    void shutdownSocket(int);
    bool isValidSocket();
    int receiveData(void* data, ssize_t length);
    int sendData(const void* buffer, ssize_t length);
    void setNonBlocking(bool nonBlocking);
    void setTimeOut(int seconds);
    ~Socket();
};