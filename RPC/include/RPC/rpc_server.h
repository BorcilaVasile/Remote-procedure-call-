#include <unistd.h>
#include <thread>
#include <queue>
#include <future>   
#include <condition_variable>
#include <random>
#include <sstream>
#include <fstream>
#include <typeinfo> 
#include <RPC/encryption.h>
#include <RPC/server_socket.h>
#include <RPC/procedure_format.pb.h>

struct ClientData{
    Socket* client_socket;
    SSL* ssl;
    std::string token;
};

class Server{
private: 
    ServerSocket* server_socket;
    std::vector<std::thread> thread_pool; 
    std::queue<ClientData> client_queue;
    std::vector<std::pair<std::string, std::string>> clients;              //<username, password>
    std::unordered_map<int, std::string> client_tokens;
    std::mutex mutexLock;
    std::condition_variable condition;
    bool shutdown_request=false;
    bool useTLS=true;
    SSL_CTX* ctx;


    std::future<void> acceptConnectionsOnServer();
    SSL_CTX* createContext();
    void configureContext(SSL_CTX* ctx);
    RPC::AuthResponse authenticateClient(std::string username, std::string password, int uid, int guid,std::string token);
    std::string generateUniqueToken();
    void verifyRequestCredentials(std::string token, int uid);
    bool verifyAuthentificationCredentials(std::string username, std::string password);

    using FunctionHandler = std::function<RPC::ReturnValue(std::vector<RPC::Argument>)>;
    std::map<std::string, FunctionHandler> function_map;

    int** convertFromMatrix(const RPC::Matrix& matrix);
    RPC::Matrix convertToMatrix(int** array, int dimension);
    
public: 
    Server(size_t pool_size=10,std::string ip="0.0.0.0", uint16_t port=8080);
    void loadClients(std::string filename);
    void start();
    void cleanup();
    void receiveMessage(Socket* client_socket,char* message, int* length,SSL* ssl);

    void workerThread();
    void handleClient(Socket* client_socket, SSL* ssl,std::string token);


    //procedures
    RPC::ReturnValue sayHello(std::vector<RPC::Argument> args);
    RPC::ReturnValue returnTypeName(std::vector<RPC::Argument> args);
    RPC::ReturnValue multiplyMatrix(std::vector<RPC::Argument> args);
    RPC::ReturnValue open(std::vector<RPC::Argument> args);
    RPC::ReturnValue read(std::vector<RPC::Argument> args);
    RPC::ReturnValue write(std::vector<RPC::Argument> args);
    RPC::ReturnValue close(std::vector<RPC::Argument> args);


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