#include <RPC/rpc_server.h>

Server::Server(size_t pool_size,std::string ip, uint16_t port){
    try{
    server_socket=new ServerSocket(ip, port);
    server_socket->bindSocket();
    server_socket->listenForConnections();
    printf("\nSocket on the server is listening for connections\n"); 

    //create the pool thread
    for(int i=0;i<pool_size;i++)
        thread_pool.emplace_back(&Server::workerThread,this);
    }catch(const std::exception& e){
        std::cerr<<"Socket server error: "<<e.what()<<std::endl;
        cleanup(); // Ensure cleanup is defined in the Server class
        exit(-1);
    }
}

RPC::AuthResponse Server::authenticateClient(std::string username, std::string password, int uid, int guid)
{
    RPC::AuthResponse auth_response;
    if(username=="username" && password=="password" && uid>=1000 && guid>=1000)
    {   
        auth_response.set_status(RPC::Status::OK);
        auth_response.set_token("generated_token");
        auth_response.set_message("Authentificated succesfully to the server.");
        auth_response.set_session_expiry(4000);
    }else{
        auth_response.set_status(RPC::Status::PERMISSION_DENIED);
        auth_response.set_message("Authentification failed due to the invalid credentials");
    }
    return auth_response;
}

void Server::start(){
    try{
    acceptConnectionsOnServer();
    }catch(const std::exception& e){
        std::cerr<<"Error starting server: "<<e.what()<<std::endl;
        cleanup();
    }
}

//accept connections on the server asinchronous
std::future<void> Server::acceptConnectionsOnServer(){
    return std::async(std::launch::async, [this](){
    while(true){
        try{
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
        }catch(const std::exception& e){
            std::cerr<<"Error accepting connections on server: "<<e.what()<<std::endl;
        }
    }
    });
}

void Server::receiveMessage(Socket* client_socket, char* message, int length){
    if(client_socket==nullptr){
        throw RPCException("No client connected");
    }
    if(client_socket->receiveData(message, length)==-1)
        throw RPCException("Failed to receive message from the client");
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
        try{
            if(bytes_received==-1)
                throw RPCException("Failed to receive data from the client"); 

            RPC::Request request;
            if(!request.ParseFromArray(buffer, bytes_received))
                throw std::runtime_error("Failed to parse request message"); 

            if(request.has_function_request()){
                RPC::Response response=processRequest(request);
                sendResult(client_socket, response);

                if(request.function_request().function_name()=="disconnect")
                    break;
            }
            else if(request.has_auth_request()){

                std::string username=request.auth_request().client_id().c_str();
                std::string password=request.auth_request().client_secret().c_str();
                int uid=request.auth_request().uid();
                int gid=request.auth_request().gid();

                RPC::AuthResponse auth_response=authenticateClient(username, password, uid, gid);
                RPC::Response response;
                *response.mutable_auth_response()=auth_response;
                sendResult(client_socket, response);

                if(auth_response.status()!=RPC::Status::OK)
                    throw RPCException("Authentification failed due to invalid credentials");

            }
        }catch(RPCException& e){
            std::cerr<<"RPC error: "<<e.what()<<std::endl;
            closeConnection(client_socket);
            printf("Client %d disconnected\n",number);
            return; 
        }catch(std::exception& e){
            std::cerr<<"Error handling the client: "<<e.what()<<std::endl;
            closeConnection(client_socket);
            printf("Client %d disconnected\n",number);
            return;
        }
    }
    printf("Client %d disconnected\n",number);
    closeConnection(client_socket);
}


RPC::Response Server::processRequest(RPC::Request& request){
    RPC::Response response; 
    
    if(request.function_request().function_name()=="sayHello"){
        std::string argument_value=request.function_request().args(0).string_val();

        //configurare mesaj de raspuns
        RPC::ReturnValue* return_value=response.mutable_return_value();
        return_value->set_status(RPC::Status::OK);
        return_value->set_string_result("Hello "+argument_value);
    } else if(request.function_request().function_name()=="disconnect")
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
    if(client_socket==nullptr)
        throw RPCException("No client connected");

    int size= response.ByteSizeLong();
    std::vector<char> buffer(size);
    if(!response.SerializeToArray(buffer.data(),size))
        throw std::runtime_error("Failed to serialize response message"); 

    if(client_socket->sendData(buffer.data(),size)==-1)
        throw RPCException("Failed to send response to client"); 
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
    if(client_socket==nullptr)
        throw RPCException("No client connected");
    if(client_socket->sendData(message, length)==-1)
        throw RPCException("Failed to send message to the client");
}

void Server::cleanup() {
    if (server_socket) {
        delete server_socket;
        server_socket = nullptr;
    }
    for (auto& thread : thread_pool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    thread_pool.clear();
    printf("Server cleanup complete\n");
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
