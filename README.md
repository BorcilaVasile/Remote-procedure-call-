# RPC - Remote Procedure Call

## Overview

This project provides a Remote Procedure Call (RPC) library that facilitates communication between client and server processes. The library supports both synchronous and asynchronous modes of operation, allowing for flexible and efficient remote function calls.

## Features

- **Client-Server Communication**: Establishes connections using sockets and supports multiple client connections.
- **Synchronous and Asynchronous RPC**: Allows clients to make blocking or non-blocking remote function calls.
- **Error Handling**: Provides comprehensive error handling and messaging.
- **Security**: Supports authentication and encryption using SSL/TLS.
- **Matrix Operations**: Includes functions for matrix multiplication and other operations.
- **File Operations**: Supports remote file operations such as open, read, write, and close.

## Installation

### Prerequisites

- **Protobuf**: Protocol Buffers for serialization.
- **OpenSSL**: For secure communication.

### Steps

1. **Download the last release**
[project latest release](https://github.com/BorcilaVasile/Remote-procedure-call-/releases/tag/RPC_libraryVersion1).


2. **Extract the archive**:

3. **Open a new terminal and move to the folder which contains the extracted archive**:

4. **Install the library and the dependencies**
    ```sh
    sudo ./install_rpc.sh
    ```

## Usage 

### Example Client code

```cpp
#include <iostream>
#include <RPC/rpc_client.h>


int main() {
    // Create a new client
    Client* client = new Client(true);
    // Connect the client to the server
    client->connectToServer("0.0.0.0", 8000);
    // Authenticate the client
    client->authenticate("username1", "password1");

    // Simple function
    std::cout << "sayHello:\n" << client->sayHello("Jasmine") << std::endl;

    // File operations
    // Open a file on the server
    int fd = client->open("test.txt", O_RDONLY);
    std::cout << "File descriptor: " << fd << std::endl;
    char buffer[1024];
    std::strcpy(buffer, "Hello World!");

    // Write in a file on the server
    ssize_t bytesWrite = client->write(fd, buffer, std::strlen(buffer));
    if (bytesWrite == -1)
        std::cerr << "Error writing to file: " << std::strerror(errno) << std::endl;

    // Read from a file on the server
    ssize_t bytesRead = client->read(fd, buffer, 1020);
    if (bytesRead == -1) {
        std::cerr << "Error reading from file: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Text from file: " << buffer << std::endl;
    }

    // Close the file on the server
    int result = client->close(fd);
    if (result == -1) {
        std::cerr << "Error closing file: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "File closed successfully" << std::endl;
    }

    // Delete the client
    delete client;
    return 0;
}
```

## Example server code 
```cpp
#include <stdio.h>
#include <stdlib.h> 

#include <RPC/rpc_server.h>

int main() {
    // Create the Server
    Server* server = new Server(10,"0.0.0.0", 8000);
    // Load the clients using the specified format
    server->loadClients("Clients.txt");

    printf("Starting server...\n");
    // Start server
    server->start();
    
    // Delete server
    delete server;
    printf("Server stopped.\n");
    return 0;
}
```