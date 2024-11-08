#include <stdio.h>
#include <string>


//interface for functions which can be called
class RemoteFunction{
    public: 
        virtual std::string sayHello(std::string name)=0;
        virtual std::string disconnect()=0;
};