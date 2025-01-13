#include <RPC/socket_connection.h>

Socket::Socket(socketType type,std::string ip, uint16_t port): type(type){
    //create a socket 
    this->file_descriptor=socket(AF_INET,SOCK_STREAM, 0);
    if(this->file_descriptor==-1)
        throw RPCException("Socket creation for server failed");
    //initialize address structure
    memset(&address, 0, sizeof(address));
    //set up the type of the socket 
    if(this->type== socketType::SERVER)
        setUpServerSocket(ip,port);
    if(type==socketType::CLIENT)
        setUpClientSocket(ip, port);
}
void Socket::setAddress(std::string ip, uint16_t port)
{
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    if(inet_pton(AF_INET, ip.c_str(), &address.sin_addr.s_addr)<=0) 
        throw RPCException("Invalid IP address "+ip);

}

void Socket::setAddress(sockaddr_in address)
{
    this->address=address;
}

void Socket::setUpServerSocket(std::string ip, uint16_t port)
{
    //set up the server socket so his port and adress can be reused 
    int opt = 1;
    if(setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
        throw RPCException("Failed to set the socket options"+ std::string(strerror(errno))); 

    setAddress(ip,port);
}

void Socket::setUpClientSocket(std::string ip, uint16_t port){
   setAddress(ip, port);
}

bool Socket::isValidSocket(){
    int flags=fcntl(file_descriptor, F_GETFL);
    if(flags==-1){
        if(errno == EBADF)
            return false; 
        else
            throw RPCException("Error at fcntl"+ std::string(strerror(errno)));
    }
    return true; 
}


int Socket::receiveData(void* data, ssize_t length){
    int result = recv(file_descriptor, data, length, 0);
    if (result == -1)
        throw RPCException("Failed to receive data: " + std::string(strerror(errno)));
    if (result == 0)
        throw RPCException("Connection closed by peer");
    return result;
}

int Socket::sendData(const void* buffer, ssize_t length){
    int result = send(file_descriptor, buffer, length, 0);
    if (result == -1)
        throw RPCException("Failed to send data: " + std::string(strerror(errno)));
    return result;
}


void Socket::shutdownSocket(int how=SHUT_RDWR){
    if (isValidSocket()) {
    if (shutdown(file_descriptor, how) == -1)
        throw RPCException("Failed to shutdown socket: " + std::string(strerror(errno)));
}

}

//nonblocking state for a socket
void Socket::setNonBlocking(bool nonBlocking){
    int flags=fcntl(file_descriptor, F_GETFL,0);
    if(flags==-1)
        throw RPCException("Failed to get the socket flags");
    if(nonBlocking)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    if (fcntl(file_descriptor, F_SETFL, flags) == -1) {
        throw RPCException("Failed to set the socket flags: " + std::string(strerror(errno)));
    }

}

//timeout function which handles the time a function should wait for input
void Socket::setTimeOut(int seconds){
    struct timeval timeout; 
    timeout.tv_sec=seconds; 
    timeout.tv_usec=0; 

    if(setsockopt(file_descriptor, SOL_SOCKET,SO_RCVTIMEO,&timeout, sizeof(timeout))==-1)
        throw RPCException("Failed to set the timeout interval for receive."+ std::string(strerror(errno)));
    if(setsockopt(file_descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1)
        throw RPCException("Failed to set the timeout interval for send"+ std::string(strerror(errno)));
}

Socket::~Socket(){
    if (isValidSocket()) 
    {
        try {
            shutdownSocket();
            if (close(file_descriptor) == -1)
                std::cerr << "Failed to close socket: " << strerror(errno) << std::endl;
            file_descriptor=-1;
        } catch (const std::exception& e) {
            std::cerr << "Exception in ~Socket: " << e.what() << std::endl;
        }
    }

}
