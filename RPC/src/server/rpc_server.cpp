#include <RPC/rpc_server.h>

Server::Server(size_t pool_size,std::string ip, uint16_t port){
    //initialization of the openssl library
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    
    try{
        ctx=createContext();
        configureContext(ctx);

        server_socket=new ServerSocket(ip, port);
        server_socket->bindSocket();
        server_socket->listenForConnections();
        printf("\nSocket on the server is listening for connections\n"); 

        //create the pool thread
        for(int i=0;i<pool_size;i++)
            thread_pool.emplace_back(&Server::workerThread,this);



    }catch(const RPCException& e){
        std::cerr<<"RPC server error: "<<e.what()<<std::endl;
        cleanup();
        exit(EXIT_FAILURE);   
    }
    catch(const std::exception& e){
        std::cerr<<"Server internal error: "<<e.what()<<std::endl;
        cleanup(); 
        exit(EXIT_FAILURE);
    }
    //mapping functions to their names
    function_map["sayHello"] = [this](std::vector<RPC::Argument> args) { return sayHello(args); };
    function_map["returnTypeName"]=[this](std::vector<RPC::Argument> args){ return returnTypeName(args);};
    function_map["multiplyMatrix"]=[this](std::vector<RPC::Argument> args){return multiplyMatrix(args);};
    function_map["open"]=[this](std::vector<RPC::Argument> args){ return open(args);};
    function_map["close"]=[this](std::vector<RPC::Argument> args){ return close(args);};
    function_map["read"]=[this](std::vector<RPC::Argument> args){ return read(args);};
}

void Server::loadClients(std::string filename){
    try{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::string fullPath = std::string(cwd) + "/" + filename;

        std::cout<<std::endl<<fullPath<<std::endl;
        
        std::ifstream file(filename);
        if(!file.is_open())
            throw std::runtime_error("Failed to open the file");
        std::string line;
        while(getline(file,line))
        {
            std::istringstream iss(line);
            std::string username, password;

            iss>>username>>password;
            if(!username.empty() && !password.empty())
                clients.push_back({username, password});
        }
        file.close();

        for(std::pair<std::string, std::string> client: clients)
            std::cout<<std::endl<<"Client: "<<client.first<<" "<<client.second<<std::endl;
    }else
        throw std::runtime_error("Failed to get current working directory");
    }catch(const RPCException& e){
        std::cerr<<"RPC error loading clients: "<<e.what()<<std::endl;
        exit(EXIT_FAILURE);
    }catch(const std::exception& e){
        std::cerr<<"Internal error loading clients: "<<e.what()<<std::endl;
        exit(EXIT_FAILURE); 
    }
}

RPC::AuthResponse Server::authenticateClient(std::string username, std::string password, int uid, int guid,std::string token)
{
    RPC::AuthResponse auth_response;
    std::cout<<std::endl<<"Client: "<<username<<" "<<password<<std::endl;
    if(uid>=1000 && guid>=1000 && verifyAuthentificationCredentials(username,password))
    {   
        auth_response.set_status(RPC::Status::OK);
        auth_response.set_token(token);
        auth_response.set_message("Authentificated succesfully to the server.");
        auth_response.set_session_expiry(4000);

        client_tokens[uid]=token;
    }else{
        auth_response.set_status(RPC::Status::PERMISSION_DENIED);
        auth_response.set_message("Authentification failed due to the invalid credentials");
    }
    return auth_response;
}

void Server::verifyRequestCredentials(std::string token, int uid)
{
    auto it = client_tokens.find(uid);
    if (it == client_tokens.end() || it->second != token) {
        throw RPCException("Invalid token or client ID");
    }
}

bool Server::verifyAuthentificationCredentials(std::string username, std::string password){
    for(std::pair<std::string,std::string> client:clients)
        if(username==client.first)
        {
            if(password==client.second)
                return true;
            else 
                return false;
        }
    return false;
}
std::string Server::generateUniqueToken(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::ostringstream token;
    for (int i = 0; i < 32; ++i) {
        token << std::hex << dis(gen);
    }
    return token.str();
}

void Server::start()
{
    acceptConnectionsOnServer();
    cleanup();
}

//accept connections on the server asynchronous
std::future<void> Server::acceptConnectionsOnServer(){
    return std::async(std::launch::async, [this](){
    while(true){
        try{
            Socket* client_socket=server_socket->acceptConnections();
            //if the socket is null than the loop will continue without doing 
            //something else 
            if(client_socket==nullptr)
            {
                std::cerr<<"Failed to accept connection from client"<<std::endl;
                continue;
            }

            SSL* ssl=nullptr;
            if(useTLS){
                ssl=SSL_new(ctx);
                SSL_set_fd(ssl,client_socket->getSocketFd());
                if (SSL_accept(ssl) <= 0) {
                    ERR_print_errors_fp(stderr);
                    SSL_free(ssl);
                    delete client_socket;
                    continue;
                }
            }   
            // create a scope for the next part of code
            // so the lock will exist just temporary in this scope
            {   
                std::unique_lock<std::mutex> lock(mutexLock);
                ClientData client;
                client.client_socket=client_socket;
                client.ssl=ssl;
                client.token=generateUniqueToken();
                client_queue.push(client);
            }
            //announce the thread that the condition has been acomplished
            condition.notify_one();
        }catch(const RPCException& e){
            std::cerr<<"RPC error accepting connections on server: "<<e.what()<<std::endl;
        }
        catch(const std::exception& e){
            std::cerr<<"Error accepting connections on server: "<<e.what()<<std::endl;
        }
    }
    });
}

SSL_CTX* Server::createContext(){
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = SSLv23_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void Server::configureContext(SSL_CTX* ctx) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    EVP_PKEY* pkey = generateRSAKey();
    if (!pkey)
        throw RPCException("Failed to generate RSA private key");

    X509* x509 = generateCertificate(pkey);
    if (!x509) {
        EVP_PKEY_free(pkey); 
        throw RPCException("Failed to generate RSA public certificate");
    }

    if (SSL_CTX_use_certificate(ctx, x509) <= 0) {
        X509_free(x509);
        EVP_PKEY_free(pkey);
        throw RPCException("Failed to use the RSA public certificate");
    }

    if (SSL_CTX_use_PrivateKey(ctx, pkey) <= 0) {
        X509_free(x509);
        EVP_PKEY_free(pkey);
        throw RPCException("Failed to use the RSA private key");
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        X509_free(x509);
        EVP_PKEY_free(pkey);
        throw RPCException("Private key does not match the public certificate");
    }

    X509_free(x509);
    EVP_PKEY_free(pkey);
}

void Server::receiveMessage(Socket* client_socket, char* message, int* length,SSL* ssl){
    if(client_socket==nullptr){
        throw RPCException("No client connected");
    }
    int bytes_received; 
    if(ssl)
        bytes_received=SSL_read(ssl, message, *length);
    else
        bytes_received=client_socket->receiveData(message,*length);
    if(bytes_received==-1)
        throw RPCException("Failed to receive message from the client on socket");
    else   
        *length=bytes_received;
}

void Server::workerThread()
{
    while(true){
        Socket* client_socket; 
        SSL* ssl;
        std::string token;
        {
            std::unique_lock<std::mutex> lock(mutexLock);
            condition.wait(lock,[this] {return !client_queue.empty() || shutdown_request; });

            if(shutdown_request && client_queue.empty())
                return; 

            client_socket=client_queue.front().client_socket;
            ssl=client_queue.front().ssl;
            token=client_queue.front().token;
            client_queue.pop();
        }
         try {
            handleClient(client_socket, ssl,token);
        } catch (const std::exception& e) {
            std::cerr << "Error handling client: " << e.what() << std::endl;
        }    
    }

}
void Server::handleClient(Socket* client_socket, SSL* ssl,std::string token)
{
    while(true){
        char buffer[1024]; 
        int length=sizeof(buffer);
        try{
            receiveMessage(client_socket,buffer, &length, ssl);

            RPC::Request request;
            if(!request.ParseFromArray(buffer, length))
                throw std::runtime_error("Failed to parse request message"); 

            if(request.has_function_request()){
                RPC::Response response=processRequest(request);
                sendResponse(client_socket, response,ssl);
                if(request.function_request().function_name()=="disconnect")
                    break;
            }
            else if(request.has_auth_request()){

                std::string username=request.auth_request().client_id().c_str();
                std::string password=request.auth_request().client_secret().c_str();
                int uid=request.auth_request().uid();
                int gid=request.auth_request().gid();

                RPC::AuthResponse auth_response=authenticateClient(username, password, uid, gid,token);
                RPC::Response response;
                *response.mutable_auth_response()=auth_response;
                sendResponse(client_socket, response,ssl);

                if(auth_response.status()!=RPC::Status::OK)
                    throw RPCException("Authentification failed due to invalid credentials");
            }
        }catch(RPCException& e){
            std::cerr<<"RPC error: "<<e.what()<<std::endl;
            break;
        }catch(std::exception& e){
            std::cerr<<"Error handling the client: "<<e.what()<<std::endl;
            break;
        }
    }
    disconnectClient(client_socket);
    if(ssl)
        free(ssl);
}


RPC::Response Server::processRequest(RPC::Request& request){
    RPC::Response response; 
    RPC::ReturnValue* return_value=response.mutable_return_value();
    try{
        verifyRequestCredentials(request.function_request().token(), request.function_request().client_id());

        std::cout<<std::endl<<"Function name: "<<request.function_request().function_name()<<std::endl;
        
        auto it=function_map.find(request.function_request().function_name());
        if(it!=function_map.end())
        {
            //i make a copy of the arguments in args because i can't use just what i have from protobuffers
            std::vector<RPC::Argument> args(request.function_request().args().begin(), request.function_request().args().end());
            *return_value = it->second(args);
        }
        else {
            if(request.function_request().function_name()=="disconnect")
            {   
                return_value->set_status(RPC::Status::OK);
                return_value->set_string_result("Disconnection request received");
            }
            else{
                return_value->set_status(RPC::Status::NOT_FOUND);
                return_value->set_message("Function not found");
            }  
        }
    }catch(RPCException& e){
        std::cerr<<"RPC error: "<<e.what()<<std::endl;
        return_value->set_status(RPC::Status::INTERNAL_ERROR);
        return_value->set_message(e.what());
    }

    return response;
}

void Server::sendResponse(Socket* client_socket, RPC::Response& response,SSL* ssl){
    if(client_socket==nullptr)
        throw RPCException("No client connected");

    int size= response.ByteSizeLong();
    std::vector<char> buffer(size);
    if(!response.SerializeToArray(buffer.data(),size))
        throw std::runtime_error("Failed to serialize response message"); 

    if(ssl){
        if(SSL_write(ssl,buffer.data(),size)<=0)
            throw RPCException("Failed to send secured response to client");
    }else{
    if(client_socket->sendData(buffer.data(),size)==-1)
        throw RPCException("Failed to send response to client"); 
    }
}

void Server::closeConnection(Socket* client_socket)
{
    if(client_socket!=nullptr)
    {
        delete client_socket;
        client_socket=nullptr;      
        printf("Client connection closed\n");
    }

}

void Server::disconnectClient(Socket *client_socket)
{
    closeConnection(client_socket);
    std::cout<<"Client disconnected"<<std::endl;
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
