# High-Performance HTTP Load Balancer (C++)

A multi-threaded Layer 7 HTTP Load Balancer built from scratch in C++ using Berkeley Sockets. It distributes incoming traffic across a cluster of backend servers using a **Round-Robin** scheduling algorithm, designed to handle high concurrency without blocking.

![C++](https://img.shields.io/badge/C++-17-blue) ![Networking](https://img.shields.io/badge/Networking-TCP%2FIP-green) ![Concurrency](https://img.shields.io/badge/Concurrency-Multithreading-orange)

## 🚀 Features
* **Layer 7 Load Balancing:** Parses HTTP/1.1 requests and forwards them to healthy backends.
* **Round-Robin Scheduling:** Distributes traffic evenly across available servers to prevent overload.
* **Multi-Threading:** Uses `std::thread` to handle each client connection concurrently, ensuring non-blocking I/O.
* **Connection Management:** Manages full TCP lifecycles (Socket creation, Binding, Listening, Accepting) manually.

## 🛠 Tech Stack
* **Language:** C++17
* **Networking:** BSD Sockets (`sys/socket.h`, `netinet/in.h`)
* **Concurrency:** C++ Standard Threading Library
* **Build System:** CMake

## 🏃‍♂️ How to Run

### 1. Start the Mock Backend Cluster
This Python script simulates 3 backend web servers listening on ports 8081, 8082, and 8083.
```bash
python3 mock_servers.py