#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <RPC/rpc_client.h>

void printMatrix(int** matrix, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void freeMatrix(int** matrix, int dimension) {
    for (int i = 0; i < dimension; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}


int main(){
    Client* client = new Client(true);
    client->connectToServer("0.0.0.0", 8000);
    client->authenticate("username1", "password1");

    //simple function
    std::cout << "sayHello:\n" <<client->sayHello("Jasmine") << std::endl;

    //functie inexistenta
    std::cout<<"nonExistentFunction: "<<std::endl;
    client->nonExistentFunction();

    //pointer type arguments function
    char* message=(char*)malloc(4055*sizeof(char));
    std::cout<<"\nreturnTypeNameAndSize:\n"<<client->returnTypeName(message)<<std::endl;

    //aritmetic operation function
    int dimension = 3;

    int** A = new int*[dimension];
    int** B = new int*[dimension];
    for (int i = 0; i < dimension; i++) {
        A[i] = new int[dimension];
        B[i] = new int[dimension];
    }

    A[0][0] = 1; A[0][1] = 1; A[0][2] = 1;
    A[1][0] = 1; A[1][1] = 1; A[1][2] = 1;
    A[2][0] = 1; A[2][1] = 1; A[2][2] = 1;

    B[0][0] = 1; B[0][1] = 1; B[0][2] = 1;
    B[1][0] = 1; B[1][1] = 1; B[1][2] = 1;
    B[2][0] = 1; B[2][1] = 1; B[2][2] = 1;
    
    int** C=client->multiplyMatrix(A,B,dimension);

    printMatrix(C,dimension);

    freeMatrix(A,dimension);
    freeMatrix(B,dimension);
    freeMatrix(C,dimension);

    //file functions
     // open file
    int fd = client->open("test.txt", O_RDONLY);
    std::cout << "Valoarea file descriptorului: " << fd << std::endl;
    char buffer[1024];
    std::strcpy(buffer, "Hello World!");

    //write to file 
    std::cout<<"Descriptor value: "<<fd<<std::endl;
    ssize_t bytesWrite =client->write(fd,buffer,std::strlen(buffer));
    if(bytesWrite==-1)
        std::cerr<<"Eroare la scrierea in fisier: "<<std::strerror(errno)<<std::endl;

    // read from file
    ssize_t bytesRead = client->read(fd, buffer, 1020);
    if (bytesRead == -1) {
        std::cerr << "Eroare la citirea din fișier: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Text from Clients.txt: " << buffer << std::endl;
    }



    // close file
    int result = client->close(fd);
    if (result == -1) {
        std::cerr << "Eroare la închiderea fișierului: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Fișier închis cu succes" << std::endl;
    }

    //disconnect function
    std::string disconnectFuture = client->disconnect();
    std::cout << "disconnect:"<<std::endl<< disconnectFuture<< std::endl;
    
    delete client;
    return 0;
}



