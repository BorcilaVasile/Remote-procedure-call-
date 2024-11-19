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
    std::queue<std::pair<Socket*,SSL*>> client_queue;
    std::mutex mutexLock;
    std::condition_variable condition;
    bool shutdown_request=false;
    bool useTLS=true;
    SSL_CTX* ctx;


    std::future<void> acceptConnectionsOnServer();
    SSL_CTX* createContext();
    void configureContext(SSL_CTX* ctx);
    RPC::AuthResponse authenticateClient(std::string username, std::string password, int uid, int guid);
public: 
    Server(size_t pool_size=10,std::string ip="0.0.0.0", uint16_t port=8080);
    void start();
    void cleanup();
    void receiveMessage(Socket* client_socket,char* message, int* length,SSL* ssl);

    void workerThread();
    void handleClient(Socket* client_socket, SSL* ssl);

    RPC::Response processRequest(RPC::Request& request);
    void sendResponse(Socket* client_socket, RPC::Response& response,SSL* ssl);
    void closeConnection(Socket* client_socket);
    void disconnectClient(Socket* client_socket);
    void shutdown();
    ~Server(){
        SSL_CTX_free(ctx);
        delete server_socket;
    }
};