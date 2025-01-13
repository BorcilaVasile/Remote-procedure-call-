#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <fstream>
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

void clientTask(int client_id, const std::string& username, const std::string& password) {
    try {
        Client client(true);  // creează un client nou
        client.connectToServer("0.0.0.0", 8000);
        client.authenticate(username, password);

        // fiecare client execută mai multe operațiuni
        std::cout << "[Client " << client_id << "] sayHello: " 
                  << client.sayHello("Client " + std::to_string(client_id)) << std::endl;

        // operatiuni de matrice
        int dimension = 2;
        int** A = new int*[dimension];
        int** B = new int*[dimension];
        for (int i = 0; i < dimension; i++) {
            A[i] = new int[dimension] {1, 2};
            B[i] = new int[dimension] {3, 4};
        }

        int** result = client.multiplyMatrix(A, B, dimension);

        std::cout << "[Client " << client_id << "] Matrix multiplication result:" << std::endl;
        printMatrix(result, dimension);

        freeMatrix(A, dimension);
        freeMatrix(B, dimension);
        freeMatrix(result, dimension);

        // operațiuni pe fișiere
        std::string filename = "test_file_" + std::to_string(client_id) + ".txt";

        // deschidere fișier
        int fd = client.open(filename.c_str(), O_CREAT | O_RDWR);
        if (fd == -1) {
            std::cerr << "[Client " << client_id << "] Error opening file: " << strerror(errno) << std::endl;
            return;
        }

        // scriere în fișier
        std::string write_data = "Hello from Client " + std::to_string(client_id);
        ssize_t bytes_written = client.write(fd, write_data.c_str(), static_cast<size_t>(write_data.length()));
        if (bytes_written == -1) {
            std::cerr << "[Client " << client_id << "] Error writing to file: " << strerror(errno) << std::endl;
        } else {
            std::cout << "[Client " << client_id << "] Successfully wrote to file." << std::endl;
        }

        // citire din fișier
        char buffer[1024] = {0};
        ssize_t bytes_read = client.read(fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            std::cerr << "[Client " << client_id << "] Error reading from file: " << strerror(errno) << std::endl;
        } else {
            std::cout << "[Client " << client_id << "] Read from file: " << buffer << std::endl;
        }

        // închidere fișier
        if (client.close(fd) == -1) {
            std::cerr << "[Client " << client_id << "] Error closing file: " << strerror(errno) << std::endl;
        } else {
            std::cout << "[Client " << client_id << "] File closed successfully." << std::endl;
        }

        // deconectare
        std::cout << "[Client " << client_id << "] disconnect: " 
                  << client.disconnect() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Client " << client_id << "] Error: " << e.what() << std::endl;
    }
}

// Main Test
int main() {
    const int num_clients = 5;  
    std::vector<std::thread> client_threads;

    std::cout << "Starting test with " << num_clients << " clients...\n";

    for (int i = 0; i < num_clients; i++) {
        client_threads.emplace_back(clientTask, i, "username" + std::to_string(i + 1), "password" + std::to_string(i + 1));
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    std::cout << "All clients have completed their tasks.\n";
    return 0;
}
