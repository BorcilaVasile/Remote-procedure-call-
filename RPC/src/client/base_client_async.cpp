#include <RPC/base_client.h>

std::future<void> BaseClient::connectToServerAsync(std::string ip, uint16_t port){
    return std::async(std::launch::async, [this, ip, port](){
            client_socket->connectToServer(ip,port);
            if(useTLS){
                ssl = SSL_new(ctx);
                SSL_set_fd(ssl, client_socket->getSocketFd());
                if (SSL_connect(ssl) <= 0) {
                    ERR_print_errors_fp(stderr);
                    exit(-1);
                }
            }
    });
}

std::future<void> BaseClient::sendDataAsync(char *message, int length)
{
    return std::async(std::launch::async, [this, message, length](){
            int bytes_sent;
            if(useTLS)
                bytes_sent=SSL_write(ssl, message, length);
            else
                bytes_sent=client_socket->sendData(message, length);
            if(bytes_sent==-1)
                throw RPCException("Failed to send data to server");
    });
}

std::future<void> BaseClient::sendRequestAsync(RPC::Request& request){
    return std::async(std::launch::async, [this,request](){
            int size=request.ByteSizeLong();
            char* buffer=new char[size];
            if(!request.SerializeToArray(buffer,size)){
                delete[] buffer;
                throw std::runtime_error("Failed to serialize request");
            }
            this->sendDataAsync(buffer, size);
            delete[] buffer;
    });
}

std::future<void> BaseClient::receiveDataAsync(char* message, int* length){
     return std::async(std::launch::async, [this,message,length](){
        int bytes_received; 
        if(useTLS)
            bytes_received=SSL_read(ssl, message, *length);
        else
            bytes_received=client_socket->receiveData(message, *length);
        
        if(bytes_received<=0)
            throw RPCException("Failed to receive data from server");
        else
            *length=bytes_received;
    });
}


std::future<RPC::Response> BaseClient::receiveResponseAsync(){
    return std::async(std::launch::async, [this]()->RPC::Response {
        char buffer[1024];
        int length=sizeof(buffer);
        RPC::Response response;

            this->receiveDataAsync(buffer, &length);
            if(!response.ParseFromArray(buffer,length))
                throw std::runtime_error("Failed to parse response"); 
        return response; 
    });
}


