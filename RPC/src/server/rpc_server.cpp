#include <RPC/rpc_server.h>

Server::Server(size_t pool_size,std::string ip, uint16_t port){
    server_socket=new ServerSocket(ip, port);
    server_socket->bindSocket();
    server_socket->listenForConnections();
    printf("\nSocket on the server is listening for connections\n"); 

    //create the pool thread
    for(int i=0;i<pool_size;i++)
        thread_pool.emplace_back(&Server::workerThread,this);
}

void Server::start(){
    acceptConnectionsOnServer();
}

void Server::acceptConnectionsOnServer(){
    while(true){
        Socket* client_socket=server_socket->acceptConnections();
        //if the socket is null than the loop will continue without doing 
        //something else 
        if(client_socket==nullptr)
        {
            fprintf(stderr,"Failed to accept connection from client\n");
            continue;
        }
        // create a scope for the next part of code
        // so the lock will exist just temporary in this scope

        {   
            countClients++;
            printf("Client %d connected succesfully\n", countClients);
            std::unique_lock<std::mutex> lock(mutexLock);
            client_queue.push(std::make_pair(client_socket,countClients));
        }
        //announce the thread that the conditiion has been acomplished
        condition.notify_one();
    }
}

void Server::receiveMessage(Socket* client_socket, char* message, int length){
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

void Server::workerThread()
{
    while(true){
        Socket* client_socket; 
        int number;
        {
            std::unique_lock<std::mutex> lock(mutexLock);
            condition.wait(lock,[this] {return !client_queue.empty() || shutdown_request; });

            if(shutdown_request && client_queue.empty())
                return; 

            client_socket=client_queue.front().first;
            number=client_queue.front().second;
            client_queue.pop();
        }
        handleClient(client_socket, number);    
    }

}
void Server::handleClient(Socket* client_socket, int number)
{
    while(true){
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
        sendResult(client_socket, response);

        if(request.function_name()=="disconnect")
            break;
    }
    printf("Client %d disconnected\n",number);
    closeConnection(client_socket);
}


RPC::Response Server::processRequest(RPC::Request& request){
    RPC::Response response; 
    
    if(request.function_name()=="sayHello"){
        std::string argument_value=request.args(0).string_val();

        //configurare mesaj de raspuns
        RPC::ReturnValue* return_value=response.mutable_return_value();
        return_value->set_status(RPC::Status::OK);
        return_value->set_string_result("Hello "+argument_value);
    } else if(request.function_name()=="disconnect")
    {
         RPC::ReturnValue* return_value=response.mutable_return_value();
        return_value->set_status(RPC::Status::OK);
        return_value->set_string_result("Disconnection request received");
    }
    else{
        response.mutable_return_value()->set_status(RPC::Status::NOT_FOUND);
        response.mutable_return_value()->set_message("Function not found");
    }   

    return response;
}

void Server::sendResult(Socket* client_socket, RPC::Response& response){
    if(client_socket==nullptr){
        fail("No client connected");
        return;
    }

    int size= response.ByteSizeLong();
    std::vector<char> buffer(size);
    if(!response.SerializeToArray(buffer.data(),size)){
        fail("Failed to serialize response message"); 
        return; 
    }

    if(client_socket->sendData(buffer.data(),size)==-1){
        fail("Failed to send response to client"); 
    }
}

void Server::closeConnection(Socket* client_socket)
{
    if(client_socket!=nullptr)
    {
        countClients--;
        delete client_socket;
        client_socket=nullptr;      
        printf("Client connection closed\n");
    }

}

void Server::sendMessage(Socket* client_socket, char *message, int length)
{
    if(client_socket==nullptr){
        fail("No client connected");
        return; 
    }
    if(client_socket->sendData(message, length)==-1)
        fail("Failed to send message to the client");
}

void Server::shutdown(){
    {
        std::unique_lock<std::mutex> lock(mutexLock);
        shutdown_request=true;
    }
    condition.notify_all(); 
    
    for (auto& thread : thread_pool) {
        if (thread.joinable()) {
            thread.join(); 
        }
    }
    printf("Server shutdown complete\n");
}
