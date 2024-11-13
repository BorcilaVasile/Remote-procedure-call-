#include <RPC/base_client.h>

BaseClient::BaseClient(){
    client_socket=new ClientSocket();
    printf("\nThe client is ready to connect to the server\n"); 
}

bool BaseClient::connectToServer(std::string ip,uint16_t port){
        if(client_socket->connectToServer(ip, port))
        {
        printf("The client connected successfully to the server\n");
        return true; 
        }else
        {
        printf("Failed to connect to server: %s\n");
        return false; 
        }
}

bool BaseClient::authenticateUser(std::string username, std::string password, int uid, int gid)
{
    RPC::AuthRequest auth_request;
    auth_request.set_client_id(username);
    auth_request.set_client_secret(password);
    auth_request.set_uid(uid);
    auth_request.set_gid(gid);

    std::cout << "Sending authentication request...\n";

    RPC::Request request; 
    *request.mutable_auth_request() = auth_request;
    sendData(request);
    RPC::Response response = receiveResponse();

    std::cout << "Received response for authentication\n";
    if (response.auth_response().status() == RPC::Status::OK) {
        return true;
    } else {
        std::cerr<<"\nFailed to authenticate in the server\n";
        return false;
    }
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