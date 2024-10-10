#ifndef _UDP_CLIENT_HPP_
#define _UDP_CLIENT_HPP_

#include <iostream>
#include <string>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define USAGE_ERR   1
#define BIND_ERR    2
#define SOCKET_ERR  3
#define OPEN_ERR    4

namespace client{ 
    class UDPClient {
    public:
        UDPClient(const std::string& address, uint16_t port)
            : _address(address),
              _port(port),
              _sockfd(-1)
        {}


        // 创建套接字
        bool createSocket(){
            // DGram socket 适用于UDP
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            
            if (_sockfd < 0) {
                std::cerr << "Failed to create socket. errno: " << errno << std::endl;
                return false;
            }
            std::cout << "socket success: " << " : " << _sockfd << std::endl;
            return true;
        }

        static void* readMessage(void* args)
        {   
            pthread_detach(pthread_self());
            int sockfd = *static_cast<int*>(args);

            while(1)
            {
                // 接收服务器的响应
                char buffer[1024];
                struct sockaddr_in temp;
                socklen_t len = sizeof(temp);
                ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&temp, &len);
                if (n < 0) {
                    std::cerr << "Failed to receive message. errno: " << errno << std::endl;
                    exit(SOCKET_ERR);
                }
                buffer[n] = '\0';
                std::cout << "服务器处理结果 # " << buffer << std::endl;
            }

        }

        void run()
        {
            pthread_create(&_reader, NULL, readMessage, (void*)&_sockfd);

            struct sockaddr_in _serverAddr;
            memset(&_serverAddr, 0, sizeof(_serverAddr));
            _serverAddr.sin_family = AF_INET;
            _serverAddr.sin_port = htons(_port);
            _serverAddr.sin_addr.s_addr = inet_addr(_address.c_str());
            std::string message;
            while(1){
                fprintf(stderr, "Please enter message # ");
                fflush(stderr);
                std::getline(std::cin, message);
                ssize_t s = sendto(_sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr));
                if (s < 0)
                {
                    std::cerr << "Failed to send message. errno: " << errno << std::endl;
                    exit(SOCKET_ERR);
                }
                
            }
        }

        ~UDPClient(){
            if (_sockfd >= 0) {
                close(_sockfd);
            }
        }


    private:
        std::string _address;
        uint16_t _port;
        int _sockfd;
        pthread_t _reader;
    };
}

#endif // _UDP_CLIENT_HPP_