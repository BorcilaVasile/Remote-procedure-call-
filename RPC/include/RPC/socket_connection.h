#include <sys/socket.h>

enum class socketType{
    SERVER, CLIENT
};
class Socket{ 
private: 
    int file_descriptor;
public:
    Socket(socketType type);
    void createSocketForServer();
    void createSocketForClient();
};