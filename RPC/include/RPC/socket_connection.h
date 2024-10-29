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
private: 
    int file_descriptor;
    socketType type;
    struct sockaddr_in address;
private: 
    void setUpServerSocket(uint16_t port=8080);
    void setUpClientSocket(std::string ip="127.0.0.1", uint16_t port=8080);
public:
    Socket(socketType type,std::string ip="127.0.0.1", uint16_t port=8080);
    void setAdress(std::string ip, uint16_t port);
    void bindSocket();
    void listenForConnections();
    void connectToServer();
    Socket* acceptConnections();
    void shutdownSocket(int);
    bool isValidSocket();
    int receiveData(void* data, ssize_t length);
    int sendData(const void* buffer, ssize_t length);
    void setNonBlocking(bool nonBlocking);
    void setTimeOut(int seconds);
    ~Socket();
};