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
            if(this->client_socket->sendData(message,length)==-1)
                throw RPCException("Failed to send data to server");
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
                throw RPCException("Failed to send data to server");
            }
            delete[] buffer;
    });
}

std::future<void> BaseClient::receiveDataAsync(char* message, int length){
    return std::async(std::launch::async, [this,message,length](){
        if(client_socket->receiveData(message,length)==-1)
            throw RPCException("Failed to receive data from server");
    });
}

std::future<RPC::Response> BaseClient::receiveResponseAsync(){
    return std::async(std::launch::async, [this]()->RPC::Response {
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
    });
}