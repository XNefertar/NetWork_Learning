#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>


#define USAGE_ERR   1
#define BIND_ERR    2
#define SOCKET_ERR  3
#define OPEN_ERR    4


namespace server{
    class UDPServer {
    public:
        UDPServer(int port, const std::string& address = "0.0.0.0")
            : _address(address), _port(port), _sockfd(-1)
        {}

        void run(){
            // 服务器的本质是一个死循环
            for(;;){
                receiveMessages();
            }
        }

        // 创建套接字
        // 
        bool initSocket(){
            // DGram socket 适用于UDP
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd < 0) {
                std::cerr << "Failed to create socket. errno: " << errno << std::endl;
                return false;
            }
            sockaddr_in _serverAddr;
            memset(&_serverAddr, 0, sizeof(_serverAddr));
            _serverAddr.sin_family = AF_INET;
            _serverAddr.sin_port = htons(_port);
            _serverAddr.sin_addr.s_addr = inet_addr(_address.c_str());

            // bind绑定
            int n = bind(_sockfd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr));
            if (n < 0) {
                std::cerr << "Failed to bind socket. errno: " << errno << std::endl;
                exit(BIND_ERR);
            }
            return true;
        }

        // 接收消息
        void receiveMessages(){
            struct sockaddr_in _clientAddr;
            socklen_t _clientAddrLen = sizeof(_clientAddr);
            char buffer[1024];
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&_clientAddr, &_clientAddrLen);
            if (s < 0) {
                std::cerr << "Failed to receive message. errno: " << errno << std::endl;
                return;
            }
            buffer[s] = '\0';
            std::cout << "Client[" << inet_ntoa(_clientAddr.sin_addr) << "]" << " : " << ntohs(_clientAddr.sin_port) << " # " << buffer << std::endl;
        }

        ~UDPServer(){
            if (_sockfd >= 0) {
                close(_sockfd);
            }
        }

    private:
        std::string _address;
        int _port;
        int _sockfd;
        // struct sockaddr_in serverAddr_, clientAddr_;
    };

}
#endif // UDP_SERVER_HPP