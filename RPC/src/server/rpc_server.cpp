#include <RPC/rpc_server.h>

Server::Server(){
    server_socket=new Socket(socketType::SERVER);
    server_socket->bindSocket();
    server_socket->listenForConnections();
    printf("\nSocket on the server is listening for connections\n"); 
}

void Server::acceptConnectionsOnServer(){
    Socket* client_socket = server_socket->acceptConnections();
    if (client_socket == nullptr) {
        fail("Failed to accept connection from client");
        return;
    }

    printf("Client connected\n");
    this->client_socket = client_socket;
}

void Server::receiveMessage(char* message, int length){
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }
    if(client_socket->receiveData(message, length)==-1)
    {
        printf("%s\n", message);
        fail("Failed to receive message from the client");
    }
}   

void Server::receiveFunction(){
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }

    char buffer[1024]; 
    int bytes_received=client_socket->receiveData(buffer,sizeof(buffer));
    if(bytes_received==-1){
        fail("Failed to receive data from the client"); 
        return; 
    }
    RPC::Request request;
    if(!request.ParseFromArray(buffer, bytes_received)){
        fail("Failed to parse request message"); 
        return; 
    }

    RPC::Response response=processRequest(request);
    sendResult(response);
}

RPC::Response Server::processRequest(RPC::Request& request){
    RPC::Response response; 
    
    if(request.function_name()=="sayHello"){
        std::string argument_value=request.args(0).string_val();

        //configurare mesaj de raspuns
        RPC::ReturnValue* return_value=response.mutable_return_value();
        return_value->set_status(RPC::Status::OK);
        return_value->set_string_result("Hello "+argument_value);
    }
    else{
        response.mutable_return_value()->set_status(RPC::Status::NOT_FOUND);
        response.mutable_return_value()->set_message("Function not found");
    }

    return response;
}

void Server::sendResult(RPC::Response& response){
    if(client_socket==nullptr){
        fail("No client connected");
        return;
    }

    int size= response.ByteSizeLong();
    char* buffer=new char[size];
    if(!response.SerializeToArray(buffer,size)){
        delete[] buffer;
        fail("Failed to serialize response message"); 
        return; 
    }

    if(client_socket->sendData(buffer,size)==-1){
        fail("Failed to send response to client"); 
    }

    delete[] buffer;
}

void Server::closeConnection()
{
    if(client_socket!=nullptr)
    {
        delete client_socket;
        client_socket=nullptr;      
        printf("Client connection closed\n");
    }

}
void Server::sendMessage(char *message, int length)
{
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }
    if(client_socket->sendData(message, length)==-1)
        fail("Failed to send message to the client");
}