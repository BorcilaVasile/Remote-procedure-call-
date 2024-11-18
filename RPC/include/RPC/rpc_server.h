#include <unistd.h>
#include <thread>
#include <queue>
#include <future>   
#include <condition_variable>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <RPC/server_socket.h>
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
    SSL_CTX* ctx;


    std::future<void> acceptConnectionsOnServer();
    SSL_CTX* createContext();
    void configureContext(SSL_CTX* ctx);
    RPC::AuthResponse authenticateClient(std::string username, std::string password, int uid, int guid);
public: 
    Server(size_t pool_size=10,std::string ip="0.0.0.0", uint16_t port=8080);
    void start();
    void cleanup();
    void sendResult(Socket* client_socket,RPC::Response& response);
    void receiveMessage(Socket* client_socket,char* message, int length);

    void workerThread();
    void handleClient(Socket* client_socket, int number);

    RPC::Response processRequest(RPC::Request& request);
    void sendMessage(Socket* client_socket,char* message,int length);
    void sendResult(RPC::Response& response);
    void closeConnection(Socket* client_socket);
    void disconnectClient(Socket* client_socket);
    void shutdown();
    ~Server(){
        SSL_CTX_free(ctx);
        delete server_socket;
    }
};