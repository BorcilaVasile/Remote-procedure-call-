#include <RPC/base_client.h>

std::future<void> BaseClient::connectToServerAsync(std::string ip, uint16_t port){
    return std::async(std::launch::async, [this, ip, port](){
        try{
            client_socket->connectToServer(ip,port);
        }catch(RPCException& e){
            std::cerr<<"RPC error: "<<e.what()<<std::endl;
            exit(-1);
        }catch(std::exception& e){
            std::cerr<<"Error: "<<e.what()<<std::endl;
            exit(-1);
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

        try{
            this->receiveDataAsync(buffer, &length);
            if(!response.ParseFromArray(buffer,length))
                throw std::runtime_error("Failed to parse response"); 
        }catch(RPCException& e){
            std::cerr<<"RPC error receiving the response from the server: "<<e.what()<<std::endl;
        }catch(std::exception& e){
            std::cerr<<"Error receiving the response from the server: "<<e.what()<<std::endl;
        }
        return response; 
    });
}