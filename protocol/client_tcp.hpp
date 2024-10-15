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
#include "log.hpp"
#include "protocol.hpp"
#include "server_tcp.hpp"

#define USAGE_ERR   1
#define BIND_ERR    2
#define SOCKET_ERR  3
#define OPEN_ERR    4

using namespace std;
using namespace Procotol;

string toString(int exitcode)
{
    switch (exitcode)
    {
    case OK:
        return "OK";
    case DIV_ZERO:
        return "DIV_ZERO";
    case MOD_ZERO:
        return "MOD_ZERO";
    case OP_ERR:
        return "OP_ERR";
    default:
        return "UNKNOWN";
    }
}

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

    void run()
    {
        string message;
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(_port);
        server_addr.sin_addr.s_addr = inet_addr(_address.c_str());
        for(;;)
        {
            socklen_t server_addr_len = sizeof(server_addr);
            std::cout << "Please enter message # ";
            getline(std::cin, message);

            // 序列化
            Request req = ParseLine(message);
            if(!req.serialize(&message))
            {
                logMessage(ERROR, "Failed to serialize message");
                exit(SOCKET_ERR);
            }
            
#ifdef TEST
            std::cout << "before length: " << message << std::endl;
#endif
            // 加上报头
            message = enLength(message);
#ifdef TEST
            std::cout << "after length: " << message << std::endl;
#endif
            write(_socketfd, message.c_str(), message.size());

            // 先读取
            std::string message;
            std::string inbuffer;

            if(!recvPackage(_socketfd, inbuffer, &message))
            {
                logMessage(ERROR, "Failed to receive message");
                exit(SOCKET_ERR);
            }

#ifdef TEST
            std::cout << "recv message: " << message << std::endl;
#endif

            std::string res_text;
            if(!deLength(message, &res_text))
                return;
            // 反序列化
            Response res;
            if(!res.deserialize(res_text))
            {
                logMessage(ERROR, "Failed to deserialize message");
                exit(SOCKET_ERR);
            }

            logMessage(res.getExitcode(), "服务器处理结果 # %d, 处理码返回[%s]", res.getResult(), toString(res.getExitcode()).c_str());
            std::cout << "服务器处理结果 # " << res.getResult() << ", 处理码返回[" << toString(res.getExitcode()) << "]" << std::endl;
        }

    }

    // 处理用户输入
    Request ParseLine(const std::string &line)
    {
        int x, y;
        char op;
        sscanf(line.c_str(), "%d %c %d", &x, &op, &y);
        return Request(x, y, op);
    }

};


#endif