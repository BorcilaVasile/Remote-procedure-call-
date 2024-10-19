#include <RPC/socket_connection.h>

Socket::Socket(socketType type): type(type){
    //create a socket 
    this->file_descriptor=socket(AF_INET,SOCK_STREAM, 0);
    if(this->file_descriptor==-1)
        fail("Error at socket creation for server");
    //set up the type of the socket 
    if(this->type== socketType::SERVER)
        setUpServerSocket();
    if(type==socketType::CLIENT)
        setUpClientSocket();
}


void Socket::setUpServerSocket(){
    //set up the server socket so his port and adress can be reused 
    int opt = 1;
    if(setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
        fail("Error setting the socket options"); 

    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(8080);
}

void Socket::setUpClientSocket(){
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(8080);
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

void Socket::acceptConnections(){
    socklen_t address_len=sizeof(address);
    file_descriptor=accept(file_descriptor,(struct sockaddr*)&address,&address_len);
    if(file_descriptor==-1)
    {
        close(file_descriptor);
        fail("Error at accepting connections on the socket server");
    }

    printf("Client connected\n");
}

Socket::~Socket(){
    if(isValidSocket())
        close(file_descriptor);
}