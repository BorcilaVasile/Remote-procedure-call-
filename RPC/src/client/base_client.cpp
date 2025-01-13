#include <RPC/base_client.h>

BaseClient::BaseClient(){
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx=createContext();
    configureContext(ctx);

    client_socket=new ClientSocket();
    if(client_socket==nullptr)
        throw RPCException("Failed to create client socket");
   
}

BaseClient::~BaseClient(){
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

void BaseClient::connectToServer(std::string ip,uint16_t port){
        client_socket->connectToServer(ip,port);
        if(useTLS){
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_socket->getSocketFd());

        if (SSL_connect(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(-1);
        }
        }
}

void BaseClient::authenticateUser(std::string username, std::string password, int uid, int gid)
{
    RPC::AuthRequest auth_request;
    auth_request.set_client_id(username);
    auth_request.set_client_secret(password);
    auth_request.set_uid(uid);
    auth_request.set_gid(gid);
    this->client_id=uid;


    RPC::Request request; 
    *request.mutable_auth_request() = auth_request;

    std::string message_request;
    request.auth_request().SerializeToString(&message_request);

    sendRequest(request);

    RPC::Response response = receiveResponse();
        
    RPC::AuthResponse auth_response=response.auth_response();

    errorHandler.handle(auth_response.status(), auth_response.message());

    std::cout<<"Client authenticated succesfully to server"<<std::endl;
    this->token.set_generatedtoken(auth_response.token().generatedtoken());
    this->token.set_permisions(auth_response.token().permisions());
        
}


void BaseClient::sendData(char *message, int length){
    int bytes_sent;
    if(useTLS)
        bytes_sent=SSL_write(ssl,message, length);
    else
        bytes_sent=client_socket->sendData(message, length);
    if(bytes_sent==-1)
        throw RPCException("Failed to send data to server");
}
void BaseClient::sendRequest(RPC::Request& request){
    int size=request.ByteSizeLong();
    char* buffer=new char[size];
    if(!request.SerializeToArray(buffer,size))
        throw std::runtime_error("Failed to serialize request");

    this->sendData(buffer, size);
   
    delete[] buffer;
}

void BaseClient::receiveData(char* message, int* length){
    int bytes_received;
    if(useTLS)
        bytes_received=SSL_read(ssl,message, *length);
    else   
        bytes_received=client_socket->receiveData(message,*length);

    if(bytes_received==-1)
        throw RPCException("Failed to receive data");
    else
        *length=bytes_received;
}

RPC::Response BaseClient::receiveResponse(){
    auto future = std::async(std::launch::async, [this]() -> RPC::Response {
        char buffer[1024];
        int length = sizeof(buffer);
        RPC::Response response;
            receiveData(buffer, &length);

            if (!response.ParseFromArray(buffer, length)) {
                throw std::runtime_error("Failed to parse response");
            }
        return response;
    });

    std::chrono::milliseconds timeout(400); // Timeout de 4 secunde
    if (future.wait_for(timeout) == std::future_status::timeout) {
        throw RPCException("Timeout while waiting for response from server");
    }

    return future.get();
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
    const char* cert_path = std::getenv("RPC_CERTIFICATE_PATH");
    if (cert_path == nullptr) {
        cert_path = "build/bin/certificate.crt"; // adaug calea implicita
        std::cout << "RPC_CERTIFICATE_PATH not set. Using default path: " << cert_path << std::endl;
    }


    if (SSL_CTX_load_verify_locations(ctx, cert_path, nullptr) <= 0) {
        ERR_print_errors_fp(stderr);
        throw RPCException("Failed to set the certificate location. Ensure 'certificate.crt' exists and is accessible.");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);  

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



RPC::Matrix BaseClient::convertToRPCMatrix(int** array, int dimension) {
    RPC::Matrix matrix;
    matrix.set_dimension(dimension);

    for (int i = 0; i < dimension; ++i) {
        RPC::Row* row = matrix.add_rows();
        for (int j = 0; j < dimension; ++j) {
            row->add_int_val(array[i][j]);
        }
    }
    return matrix;
}

int** BaseClient::convertFromRPCMatrix(const RPC::Matrix& matrix) {
    int dimension = matrix.dimension();
    int** array = new int*[dimension];
    for (int i = 0; i < dimension; ++i) {
        array[i] = new int[dimension];
        const RPC::Row& row = matrix.rows(i);
        for (int j = 0; j < row.int_val_size(); ++j) {
            array[i][j] = row.int_val(j);
        }
    }
    return array;
}


void BaseClient::checkForErrors(RPC::ReturnValue value){
    if(value.status()==RPC::Status::ERROR)
    {
        if(value.has_error_result())
            errno=value.error_result();
        std::string error_message = value.message();
        throw RPCException(error_message);
    }
}
