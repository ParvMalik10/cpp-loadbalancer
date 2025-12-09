#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

class LoadBalancer {
private:
    int server_fd;
    int port;
    std::vector<int> backend_ports = {8081, 8082, 8083}; 
    int current_backend = 0; // Round Robin Counter

public:
    LoadBalancer(int p) : port(p) {}

    void start() {
        // 1. Create Socket
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        // 2. Bind to Port 8080
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        // 3. Listen
        if (listen(server_fd, 10) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "⚖️  Load Balancer running on port " << port << std::endl;

        while (true) {
            int client_socket;
            int addrlen = sizeof(address);
            if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                continue;
            }
            
            // Handle request in a new thread (Concurrency)
            std::thread(&LoadBalancer::handle_client, this, client_socket).detach();
        }
    }

    void handle_client(int client_socket) {
        char buffer[1024] = {0};
        read(client_socket, buffer, 1024);
        
        // 4. Select Backend (Round Robin)
        int backend_port = backend_ports[current_backend];
        current_backend = (current_backend + 1) % backend_ports.size();
        
        std::cout << "Redirecting request to Backend: " << backend_port << std::endl;
        
        // 5. Connect to Backend and get Full Response
        std::string response = forward_request(backend_port);
        
        // 6. Send Response to Client
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
    }

    std::string forward_request(int backend_port) {
        int sock = 0;
        sockaddr_in serv_addr;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return "HTTP/1.1 500 Internal Server Error\r\n\r\nBackend Failed";
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(backend_port);
        
        // Convert IPv4 address from text to binary
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            return "HTTP/1.1 502 Bad Gateway\r\n\r\nBackend Connection Failed";
        }
        
        // --- THE FIX ---
        // 1. Send Request with 'Connection: close'
        // We tell the Python server to close the connection after it replies, 
        // so our read loop knows when to stop.
        std::string req = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
        send(sock, req.c_str(), req.size(), 0);
        
        // 2. Read Response Loop
        // Keep reading until the server closes connection (returns 0)
        std::string full_response;
        char buffer[4096];
        int bytes_read;
        
        while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the chunk
            full_response += buffer;
        }
        
        close(sock);
        return full_response;
    }
};