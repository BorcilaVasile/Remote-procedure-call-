#include <RPC/rpc_client.h>

Client::Client(){
    client_socket=new Socket(socketType::CLIENT);
    printf("\nThe client is ready to connect to the server\n"); 
}

void Client::connectToServer(){
    client_socket->connectToServer();
    printf("The client connected succesfully to the server\n");
}

RPC::Response Client::callFunction(std::string function_name, std::vector<RPC::Argument> args)
{
    RPC::Request request;
    request.set_function_name(function_name);
    for(const auto& arg: args){
        RPC::Argument* new_arg=request.add_args();
        *new_arg=arg;
    }

    sendData(request);
    return receiveResponse();
}

void Client::sendData(char *message, int length)
{
    if(client_socket->sendData(message,length)==-1)
        fail("Failed to send data to server");
}

void Client::sendData(RPC::Request& request){
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

RPC::Response Client::receiveResponse(){
    char buffer[1024];
    int bytes_received=client_socket->receiveData(buffer,sizeof(buffer));
    if(bytes_received==-1){
        fail("Failed to receive response from server");
    }
    RPC::Response response;
    if(!response.ParseFromArray(buffer,bytes_received)){
        throw std::runtime_error("Failed to parse response from server"); 
    }
    return response; 
}
void Client::receiveData(char* message, int length){
    if(client_socket->receiveData(message, length)==-1)
        fail("Failed to receive data from the server");
}