#include <unistd.h>
#include <thread>
#include <queue>
#include <future>   
#include <RPC/server_socket.h>
#include <condition_variable>
#include <RPC/procedure_format.pb.h>


class Server{
private: 
    ServerSocket* server_socket;
    std::vector<std::thread> thread_pool; 
    std::queue<std::pair<Socket*, int>> client_queue;
    std::mutex mutexLock;
    std::condition_variable condition;
    bool shutdown_request=false; 
    int countClients=0;


    RPC::AuthResponse authenticateClient(std::string username, std::string password, int uid, int guid);
public: 
    Server(size_t pool_size=10,std::string ip="0.0.0.0", uint16_t port=8080);
    void start();
    void cleanup();
    std::future<void> acceptConnectionsOnServer();
    void sendResult(Socket* client_socket,RPC::Response& response);
    void receiveMessage(Socket* client_socket,char* message, int length);

    void workerThread();
    void handleClient(Socket* client_socket, int number);

    RPC::Response processRequest(RPC::Request& request);
    void sendMessage(Socket* client_socket,char* message,int length);
    void sendResult(RPC::Response& response);
    void closeConnection(Socket* client_socket);
    void shutdown();
    ~Server(){
        delete server_socket;
    }
};