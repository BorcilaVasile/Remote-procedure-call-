#include <RPC/socket_connection.h>

Socket::Socket(socketType type,std::string ip="127.0.0.1", uint16_t port=8080): type(type){
    //create a socket 
    this->file_descriptor=socket(AF_INET,SOCK_STREAM, 0);
    if(this->file_descriptor==-1)
        fail("Error at socket creation for server");
    //initialize address structure
    memset(&address, 0, sizeof(address));
    //set up the type of the socket 
    if(this->type== socketType::SERVER)
        setUpServerSocket(port);
    if(type==socketType::CLIENT)
        setUpClientSocket(ip, port);
}
void Socket::setAdress(std::string ip, uint16_t port)
{
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    if(inet_pton(AF_INET, ip.c_str(), &address.sin_addr.s_addr)<=0)
        fail("Invalid IP address");

}

void Socket::setUpServerSocket(uint16_t port=8080){
    //set up the server socket so his port and adress can be reused 
    int opt = 1;
    if(setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
        fail("Error setting the socket options"); 

    setAdress("0.0.0.0",port);
}   

void Socket::setUpClientSocket(std::string ip="127.0.0.1", uint16_t port=8080){
   setAdress(ip, port);
}

void Socket::bindSocket(){
    if(bind(file_descriptor,(struct sockaddr*)&address, sizeof(address))==-1)
        fail("Error at binding the socket on the server");
}


void Socket::listenForConnections(){ 
     if(listen(file_descriptor, SOMAXCONN)==-1)
    {
        close(file_descriptor);
        fail("Error at listening on the server socket");
    }
}

bool Socket::isValidSocket(){
    int flags=fcntl(file_descriptor, F_GETFL);
    if(flags==-1){
        if(errno == EBADF)
            return false; 
        else
            fail("Error at fcntl");
    }
    return true; 
}

void Socket::connectToServer(){
    if(connect(file_descriptor, (struct sockaddr *)&address, sizeof(address))==-1)
    {
        close(file_descriptor);
        fail("Error at connecting with the server");
    }
}

Socket* Socket::acceptConnections() {
    socklen_t address_len = sizeof(address);
    int client_fd = accept(file_descriptor, (struct sockaddr*)&address, &address_len);
    if (client_fd == -1) {
        fail("Error at accepting connections on the socket server");
        return nullptr;
    }

    Socket* clientSocket = new Socket(socketType::CLIENT);
    clientSocket->file_descriptor = client_fd;
    clientSocket->address = address;
    return clientSocket;
}


int Socket::receiveData(void* data, ssize_t length){
    return recv(file_descriptor, data,length,0);
}

int Socket::sendData(const void* buffer, ssize_t length){
    return send(file_descriptor, buffer, length, 0);
}


void Socket::shutdownSocket(int how=SHUT_RDWR){
    if(isValidSocket()){
        shutdown(file_descriptor,how);
    }
}

//nonblocking state for a socket
void Socket::setNonBlocking(bool nonBlocking){
    int flags=fcntl(file_descriptor, F_GETFL,0);
    if(flags==-1)
        fail("Error getting the socket flags");
    if(nonBlocking)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    if(fcntl(file_descriptor,F_SETFL, flags)==-1)
        fail("Error setting the socket flags");
}

//timeout function which handles the time a function should wait for input
void Socket::setTimeOut(int seconds){
    struct timeval timeout; 
    timeout.tv_sec=seconds; 
    timeout.tv_usec=0; 

    if(setsockopt(file_descriptor, SOL_SOCKET,SO_RCVTIMEO,&timeout, sizeof(timeout))==-1)
        fail("Error setting the timeout interval for receive.");
    if(setsockopt(file_descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1)
        fail("Error setting the timeout interval for send");
}

Socket::~Socket(){
    if(isValidSocket())
    {
        shutdownSocket();
        close(file_descriptor);
    }
}
