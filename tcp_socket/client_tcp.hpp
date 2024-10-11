#ifndef _CLIENT_TCP_HPP
#define _CLIENT_TCP_HPP

#include <iostream>
#include <string>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>

#define USAGE_ERR   1
#define BIND_ERR    2
#define SOCKET_ERR  3
#define OPEN_ERR    4

using namespace std;
class ClientTCP
{
private:
    int _socketfd;
    int _port;
    std::string _address;
    pthread_t _reader;

public:
    ClientTCP(const std::string& address, int port, int socketfd = -1)
        : _address(address),
          _port(port),
          _socketfd(socketfd)
    {}
    ~ClientTCP() { close(_socketfd); }
    void createSocket()
    {
        _socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_socketfd < 0)
        {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        std::cout << "socket " << _socketfd << " created" << std::endl;
    }

    void connectToServer()
    {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(_port);
        server_addr.sin_addr.s_addr = inet_addr(_address.c_str());

        if(connect(_socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "Error connecting to server" << std::endl;
            exit(1);
        }
        std::cout << "connected to server" << std::endl;
    }

    // static void* readMessage(void* args)
    // {
    //     pthread_detach(pthread_self());
    //     int sockfd = *static_cast<int *>(args);

    //     while (1)
    //     {
    //         // 接收服务器的响应
    //         char buffer[1024];
    //         struct sockaddr_in temp;
    //         socklen_t len = sizeof(temp);
    //         ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&temp, &len);
    //         if (n < 0)
    //         {
    //             std::cerr << "Failed to receive message. errno: " << errno << std::endl;
    //             exit(SOCKET_ERR);
    //         }
    //         buffer[n] = '\0';
    //         std::cout << "服务器处理结果 # " << buffer << std::endl;
    //     }
    // }

    void run()
    {
        // pthread_create(&_reader, NULL, readMessage, (void *)&_socketfd);

        string message;
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(_port);
        server_addr.sin_addr.s_addr = inet_addr(_address.c_str());
        for(;;)
        {
            socklen_t server_addr_len = sizeof(server_addr);
            std::cout << "Please enter message: ";
            getline(std::cin, message);
            // sendto(_socketfd, message.c_str(), message.size(), 0, (sockaddr*)&server_addr, sizeof(server_addr));
            write(_socketfd, message.c_str(), message.size());

            // if(strcmp(message, "exit") == 0)
            // {
            //     break;
            // }
            // 接收服务器的响应
            char buffer[1024];
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            // ssize_t n = recvfrom(_socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&temp, &len);
            ssize_t n = read(_socketfd, buffer, sizeof(buffer));
            if (n == 0)
            {
                std::cerr << "Failed to receive message. errno: " << errno << std::endl;
                exit(SOCKET_ERR);
            }
            buffer[n] = '\0';
            std::cout << "服务器处理结果 # " << buffer << std::endl;
        }

    }
};




#endif