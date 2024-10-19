#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <fcntl.h>
#include <cerrno> 
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
    void setUpServerSocket();
    void setUpClientSocket();
public:
    Socket(socketType type);
    void bindSocket();
    void listenForConnections();
    void connectToServer();
    void acceptConnections();
    bool isValidSocket();
    ~Socket();
};