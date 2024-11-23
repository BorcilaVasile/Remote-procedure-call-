#include <RPC/rpc_client.h>
#include <iostream>
#include <unistd.h> // for getuid() and getgid()

void Client::authenticate(std::string username, std::string password)
{
    int uid = getuid();
    int gid = getgid();
    authenticateUser(username, password, uid, gid);
}

std::string Client::disconnect(){
    return callRemoteFunction<std::string>("disconnect");
}

int** Client::multiplyMatrix(int** A, int** B, int dimension){
    return callRemoteFunction<int**>("multiplyMatrix",dimension,A,B);
}
std::string Client::sayHello(std::string name){
    return callRemoteFunction<std::string>("sayHello", name);
}


void Client::nonExistentFunction(){
    return callRemoteFunction<void>("nonExistentFunction");
}

std::string Client::returnTypeName(void* variable){
    return callRemoteFunction<std::string>("returnTypeName",variable);
}

int Client::open(const char* pathname,int flags,int mode){
    return callRemoteFunction<int>("open", pathname, flags, mode);
}

int Client::open(const char* pathname,int flags){
    std::string path(pathname);
    return callRemoteFunction<int>("open", path, flags);
}

int Client::close(int fd){
    return callRemoteFunction<int>("close",fd);
}

ssize_t Client::read(int fd, char* buf, int count){
    std::pair<int, std::string> result = callRemoteFunction<std::pair<int, std::string>>("read", fd,buf, count);
    if (result.first > 0) 
        std::memcpy(buf, result.second.c_str(), result.first);
    return result.first;
}
ssize_t Client::read(int fd, int* buf, int count){
    return callRemoteFunction<int>("read", fd, buf, count);
}

ssize_t Client::write(int fd,char* buf, int count){
    std::string buffer=buf;
    return callRemoteFunction<int>("write",fd, buffer, count);
}

