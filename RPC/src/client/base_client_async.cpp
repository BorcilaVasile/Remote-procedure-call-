#include <RPC/base_client.h>

std::future<void> BaseClient::connectToServerAsync(std::string ip, uint16_t port){
    return std::async(std::launch::async, [this, ip, port](){
        try{
            this->client_socket->connectToServer(ip, port);
        } catch(const std::exception& e){
            printf("Connection to the server failed: %s", e.what());
            throw;
        }
    });
}

std::future<void> BaseClient::sendDataAsync(char* message, int length){
    return std::async(std::launch::async, [this, message, length](){
            if(this->client_socket->sendData(message,length)==-1)
                throw std::runtime_error("Failed to send data to server");
    });
}

std::future<void> BaseClient::sendDataAsync(RPC::Request& request){
    return std::async(std::launch::async, [this,request](){
            int size=request.ByteSizeLong();
            char* buffer=new char[size];
            if(!request.SerializeToArray(buffer,size)){
                delete[] buffer;
                throw std::runtime_error("Failed to serialize request");
            }
            if(client_socket->sendData(buffer,size)==-1)
            {
                delete[] buffer; 
                throw std::runtime_error("Failed to send data to server");
            }
            delete[] buffer;
    });
}

std::future<void> BaseClient::receiveDataAsync(char* message, int length){
    return std::async(std::launch::async, [this,message,length](){
        if(client_socket->receiveData(message,length)==-1)
            throw std::runtime_error("Failed to receive data from server");
    });
}

std::future<RPC::Response> BaseClient::receiveResponseAsync(){
    return std::async(std::launch::async, [this]()->RPC::Response {
        char buffer[1024];
        int bytes_received=client_socket->receiveData(buffer,sizeof(buffer));
        if(bytes_received==-1)
            throw std::runtime_error("Failed to receive response from server");
        RPC::Response response;
        if(!response.ParseFromArray(buffer,bytes_received)){
            throw std::runtime_error("Failed to parse response from server"); 
        }
        return response; 
    });
}