#pragma once
#pragma once
#pragma warning(disable: 4996)
#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

namespace client {
    class SocketWinClient {
    private:
        int _sockfd;
        uint16_t _port;
        std::string _ip;

    public:
        SocketWinClient(const std::string& ip = "192.168.209.135", uint16_t port = 8080, int sockfd = -1)
            :_ip(ip),
            _port(port),
            _sockfd(sockfd)
        {}

        SocketWinClient(const SocketWinClient&) = delete;
        SocketWinClient& operator=(const SocketWinClient&) = delete;
        ~SocketWinClient() = default;

        void init() {
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                std::cout << "WSAStartup failed with error: " << iResult << std::endl;
                return;
            }

            // Create socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == INVALID_SOCKET) {
                std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
                std::cout << "socket success: " << " : " << _sockfd << std::endl;
                WSACleanup();
            }

            //// Connect to server
            //sockaddr_in serverAddr;
            //memset(&serverAddr, 0, sizeof(serverAddr));
            //serverAddr.sin_family = AF_INET;
            //serverAddr.sin_port = htons(_port);
            //serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
        }

        void start() {
            sockaddr_in _serverAddr;
            memset(&_serverAddr, 0, sizeof(_serverAddr));
            _serverAddr.sin_family = AF_INET;
            _serverAddr.sin_port = htons(_port);
            _serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());

            if (connect(_sockfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR) {
                std::cout << "Error at connect(): " << WSAGetLastError() << std::endl;
                closesocket(_sockfd);
                WSACleanup();
                return;
            }
            while (1) {
                // Send message
                std::string sendbuf;
                std::cout << "Please input message # ";
                getline(std::cin, sendbuf);
                sendto(_sockfd, sendbuf.c_str(), sendbuf.length(), 0, (sockaddr*)&_serverAddr, sizeof(_serverAddr));

                // receive message
                char recvbuf[1024]{};
                int len = recvfrom(_sockfd, recvbuf, 1024, 0, NULL, NULL);
                if (len == SOCKET_ERROR) {
                    std::cout << "Error at recvfrom(): " << WSAGetLastError() << std::endl;
                    closesocket(_sockfd);
                    WSACleanup();
                    return;
                }
                std::string response = recvbuf;
                std::cout << "Response # " << response << std::endl;
            }
        }

    };
}