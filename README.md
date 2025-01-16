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
```bash
#!/bin/bash

# Definește directoarele de instalare
LIB_DIR="/usr/local/lib"
INCLUDE_DIR="/usr/local/include"
PKG_CONFIG_DIR="/usr/local/lib/pkgconfig"
DEFAULT_CERTIFICATE_PATH="/usr/local/lib/certificate.crt"

# Verifică dacă scriptul este rulat cu permisiuni de root
if [ "$EUID" -ne 0 ]; then
  echo "Te rog rulează scriptul cu permisiuni de root."
  exit 1
fi

# Verifică și instalează protobuf dacă nu este instalat
if ! dpkg -s protobuf-compiler libprotobuf-dev >/dev/null 2>&1; then
  echo "Protobuf nu este instalat. Instalare protobuf..."
  apt-get update
  apt-get install -y protobuf-compiler libprotobuf-dev
else
  echo "Protobuf este deja instalat."
fi

# Verifică și instalează openssl dacă nu este instalat
if ! dpkg -s openssl libssl-dev >/dev/null 2>&1; then
  echo "OpenSSL nu este instalat. Instalare OpenSSL..."
  apt-get update
  apt-get install -y openssl libssl-dev
else
  echo "OpenSSL este deja instalat."
fi

# Copiază fișierele de antet
echo "Copiere fișiere de antet în ${INCLUDE_DIR}..."
cp -r include/RPC/* ${INCLUDE_DIR}/

# Verifică dacă fișierul certificatului există
if [ ! -f "$DEFAULT_CERTIFICATE_PATH" ]; then
  echo "Certificatul TLS nu a fost găsit la locația implicită (${DEFAULT_CERTIFICATE_PATH})."
  echo "Creare certificat implicit pentru teste..."
  openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout "${DEFAULT_CERTIFICATE_PATH}" -out "${DEFAULT_CERTIFICATE_PATH}" \
    -subj "/C=US/ST=State/L=City/O=Organization/OU=Department/CN=example.com"
  echo "Certificatul a fost creat și salvat la ${DEFAULT_CERTIFICATE_PATH}."
else
  echo "Certificatul TLS a fost găsit la ${DEFAULT_CERTIFICATE_PATH}."
fi

# Setează permisiunile necesare pentru certificatul TLS
echo "Setare permisiuni pentru certificatul TLS..."
chmod 644 "${DEFAULT_CERTIFICATE_PATH}"
if [ $? -eq 0 ]; then
  echo "Permisiunile au fost setate cu succes: 644 (citire pentru toți utilizatorii)."
else
  echo "Eroare la setarea permisiunilor pentru certificatul TLS."
  exit 1
fi

# Setează variabila de mediu pentru certificatul TLS
if [ -z "$RPC_CERTIFICATE_PATH" ]; then
  echo "Setarea variabilei de mediu RPC_CERTIFICATE_PATH..."
  export RPC_CERTIFICATE_PATH="${DEFAULT_CERTIFICATE_PATH}"
  echo "Variabila RPC_CERTIFICATE_PATH a fost setată la ${RPC_CERTIFICATE_PATH}."
else
  echo "Variabila RPC_CERTIFICATE_PATH este deja setată la ${RPC_CERTIFICATE_PATH}."
fi

# Creează fișierul pkg-config pentru a permite utilizarea corectă în proiecte
echo "Creare fișierul pkg-config pentru libRPC..."
cat <<EOL > ${PKG_CONFIG_DIR}/libRPC.pc
prefix=${LIB_DIR}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libRPC
Description: Biblioteca RPC pentru aplicații client-server
Version: 1.0
Libs: -L\${libdir} -lRPC
Cflags: -I\${includedir}/RPC
EOL

# Actualizează cache-ul bibliotecilor dinamice
echo "Actualizare cache biblioteci dinamice..."
ldconfig

# Mesaj final
echo "Biblioteca RPC a fost instalată cu succes în sistem!"
echo "Certificatul este localizat la: ${RPC_CERTIFICATE_PATH}"

# Oferă instrucțiuni de utilizare pentru utilizatori
echo "Pentru a utiliza biblioteca în proiecte, adăugați opțiunile corespunzătoare în CMake sau Makefile:"
echo "  - CMake: target_link_libraries(proiect libRPC)"
echo "  - Makefile: -L/usr/local/lib -lRPC -I/usr/local/include"
echo "Asigurați-vă că variabila de mediu RPC_CERTIFICATE_PATH este configurată corect pentru aplicația dumneavoastră."
```

### Prerequisites

- **Protobuf**: Protocol Buffers for serialization.
- **OpenSSL**: For secure communication.

### Steps

1. **Download the last release**
[project latest release](https://github.com/BorcilaVasile/Remote-procedure-call-/releases/tag/RPC_libraryVersion1).


2. **Extract the archive**

3. **Open a new terminal and move to the folder which contains the extracted archive**

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

### Example script to compile the client application

The following script compiles the `main_client` file for the client application, linking the required libraries and including the necessary headers. Replace `your_path` with the appropriate paths to your project files.

```bash
g++ -g your_path/Remote-procedure-call-/Client/main_client.cpp your_path/Remote-procedure-call-/RPC/src/proto/procedure_format.pb.cc \
    -I your_path/Remote-procedure-call-/RPC/include            \
    -o your_path/Remote-procedure-call-/Client/main_client     \
    -L your_path/Remote-procedure-call-/build/lib/ \
    -lRPC -Wl,-rpath,your_path/Remote-procedure-call-/build/lib/ \
    -lprotobuf -lssl -lcrypto
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

### Example script to compile the server application

The following script compiles the `server_client` file for the server application, linking the required libraries and including the necessary headers. Replace `your_path` with the appropriate paths to your project files.

```bash
g++ -g your_path/Remote-procedure-call-/Server/main_server.cpp   your_path/Remote-procedure-call-/RPC/src/proto/procedure_format.pb.cc \
    -I your_path/Remote-procedure-call-/RPC/include            \
    -I your_path/Remote-procedure-call-/RPC/src/proto          \
    -o your_path/Remote-procedure-call-/Server/main_server     \
    -L your_path/Remote-procedure-call-/build/lib/      \
    -lRPC -Wl,-rpath,/home/vasile/Desktop/Remote-procedure-call-/build/lib/ \
    -lprotobuf -lssl -lcrypto
```
## Future Implementations

- **Cross-Platform Compatibility**: Extend support for macOS and Windows.
- **Advanced Error Reporting**: Implement detailed logs for better debugging.
- **Load Balancing**: Integrate support for distributing client requests across multiple servers.
- **Multi-Language Support**: Develop bindings for languages like Python and Java.
- **WebSocket Integration**: Add support for WebSocket-based communication.
- **Custom Authentication**: Enable integration with external authentication providers.
- **File Streaming**: Support for streaming large files between client and server.
- **Performance Metrics**: Add tools for monitoring and visualizing server performance in real-time.
- **Access to your own workspace on server**: Every client should have access only to their own files, regardless of the permissions they have, with the only one able to override this being the server administrator.