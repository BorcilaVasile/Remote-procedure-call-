#include <RPC/base_client.h>

BaseClient::BaseClient(){
    client_socket=new ClientSocket();
    printf("\nThe client is ready to connect to the server\n"); 
}

void BaseClient::connectToServer(std::string ip,uint16_t port){
        client_socket->connectToServer(ip,port);
        printf("The client connected succesfully to the server\n");
}

void BaseClient::sendData(char *message, int length){
    if(client_socket->sendData(message,length)==-1)
        fail("Failed to send data to server");
}

void BaseClient::sendData(RPC::Request& request){
    int size=request.ByteSizeLong();
    char* buffer=new char[size];
    if(!request.SerializeToArray(buffer,size)){
        delete[] buffer;
        throw std::runtime_error("Failed to serialize request");
    }
    if(client_socket->sendData(buffer,size)==-1)
    {
        delete[] buffer; 
        fail("Failed to send data to server");
    }
    delete[] buffer;
}

void BaseClient::receiveData(char* message, int length){
    if(client_socket->receiveData(message, length)==-1)
        fail("Failed to receive data from the server");
}

RPC::Response BaseClient::receiveResponse(){
    char buffer[1024];
    int bytes_received=client_socket->receiveData(buffer,sizeof(buffer));
    if(bytes_received==-1)
        fail("Failed to receive response from server");
    RPC::Response response;
    if(!response.ParseFromArray(buffer,bytes_received)){
        throw std::runtime_error("Failed to parse response from server"); 
    }
    return response; 
}