#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <iostream>
#include <string>
#include <functional>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>


#define USAGE_ERR   1
#define BIND_ERR    2
#define SOCKET_ERR  3
#define OPEN_ERR    4


namespace server{
    // sockfd, clientAddr, clientPort, messege
    using func_t = std::function<void(int, std::string, uint16_t, std::string)>;

    class UDPServer {
    public:
        // 加入回调函数(callback function to handle some tasks)
        UDPServer(func_t callback, int port, const std::string& address = "0.0.0.0")
            : _address(address), _port(port), _sockfd(-1), _callback(callback)
        {}

        void run(){
            char buffer[1024];
            // 服务器的本质是一个死循环
            for(;;){
                struct sockaddr_in _clientAddr;
                socklen_t _clientAddrLen = sizeof(_clientAddr);
                ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&_clientAddr, &_clientAddrLen);
                if (s < 0) {
                    std::cerr << "Failed to receive message. errno: " << errno << std::endl;
                    return;
                }
                buffer[s] = '\0';
                std::cout << "Client[" << inet_ntoa(_clientAddr.sin_addr) << "]" << " : " << ntohs(_clientAddr.sin_port) << " # " << buffer << std::endl;
                std::string messege = buffer;
                _callback(_sockfd, inet_ntoa(_clientAddr.sin_addr), ntohs(_clientAddr.sin_port), messege);
            }
        }

        // 创建套接字
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
            _serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

            // bind绑定
            int n = bind(_sockfd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr));
            if (n < 0) {
                std::cerr << "Failed to bind socket. errno: " << errno << std::endl;
                exit(BIND_ERR);
            }
            std::cout << "bind success: " << " : " << _sockfd << std::endl;
            return true;
        }

        ~UDPServer(){
            if (_sockfd >= 0) {
                close(_sockfd);
            }
        }

    private:
        func_t _callback;
        std::string _address;
        int _port;
        int _sockfd;
    };

}
#endif // UDP_SERVER_HPP