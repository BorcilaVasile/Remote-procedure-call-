#include <RPC/base_client.h>

BaseClient::BaseClient(){
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx=createContext();
    configureContext(ctx);

    client_socket=new ClientSocket();
    printf("\nThe client is ready to connect to the server\n"); 
}

BaseClient::~BaseClient(){
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

void BaseClient::connectToServer(std::string ip,uint16_t port){

    try{
        client_socket->connectToServer(ip,port);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_socket->getSocketFd());

        if (SSL_connect(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(-1);
        }
    }catch(RPCException& e){
        std::cerr<<"RPC error: "<<e.what()<<std::endl;
        exit(-1);
    }catch(std::exception& e){
        std::cerr<<"Error: "<<e.what()<<std::endl;
        exit(-1);
    }
}

void BaseClient::authenticateUser(std::string username, std::string password, int uid, int gid)
{
    try{
        RPC::AuthRequest auth_request;
        auth_request.set_client_id(username);
        auth_request.set_client_secret(password);
        auth_request.set_uid(uid);
        auth_request.set_gid(gid);

        std::cout << "Sending authentication request...\n";

        RPC::Request request; 
        *request.mutable_auth_request() = auth_request;
        std::string encrypted_request;
        //AES_encrypt()
        sendData(request);
        RPC::Response response = receiveResponse();
        
        RPC::AuthResponse auth_response=response.auth_response();

        errorHandler.handle(auth_response.status(), auth_response.message());
    }catch(RPCException& e){
        std::cerr<<"RPC error at authentification: "<<e.what()<<std::endl;
        exit(-1);
    }catch(std::exception& e){
        std::cerr<<"Error at authentification: "<<e.what()<<std::endl;
        exit(-1);
    }
}


void BaseClient::sendData(char *message, int length){
    if(client_socket->sendData(message,length)==-1)
        throw RPCException("Failed to send data to server");
}

void BaseClient::sendData(RPC::Request& request){
    int size=request.ByteSizeLong();
    char* buffer=new char[size];
    try{
        if(!request.SerializeToArray(buffer,size))
            throw std::runtime_error("Failed to serialize request");
        if(client_socket->sendData(buffer,size)==-1)
            throw RPCException("Failed to send data to server");
    }catch(RPCException& e){
        std::cerr<<"RPC error communicating with the server: "<<e.what()<<std::endl;
    }catch(std::exception& e){
        std::cerr<<"Error communicating with the server: "<<e.what()<<std::endl;
    }
    delete[] buffer;
}

void BaseClient::receiveData(char* message, int length){
    if(client_socket->receiveData(message, length)==-1)
        throw RPCException("Failed to receive data from the server");
}

RPC::Response BaseClient::receiveResponse(){
    char buffer[1024];
    int bytes_received=client_socket->receiveData(buffer,sizeof(buffer));
    RPC::Response response;
    try{
        if(bytes_received==-1)
            throw RPCException("Failed to receive response");
        if(!response.ParseFromArray(buffer,bytes_received))
            throw std::runtime_error("Failed to parse response"); 
    }catch(RPCException& e){
        std::cerr<<"RPC error receiving the response from the server: "<<e.what()<<std::endl;
    }catch(std::exception& e){
        std::cerr<<"Error receiving the response from the server: "<<e.what()<<std::endl;
    }
    return response; 
}

SSL_CTX* BaseClient::createContext() {
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void BaseClient::configureContext(SSL_CTX* ctx) {
    if (SSL_CTX_load_verify_locations(ctx, "build/bin/certificate.crt", nullptr) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}