#ifndef SOCK_HPP
#define SOCK_HPP

// 创建套接字

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include "Err.hpp"
#include "Log.hpp"

class Sock
{
    const static int backlog = 5;
public:
    Sock() = default;
    ~Sock() = default;

    static int Socket()
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
            LOG_MESSAGE(ERROR, "socket error\n");
            exit(1);
        }
        LOG_MESSAGE(DEBUG, "Socket fd: %d\n", fd);
        // 设置端口复用
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        return fd;
    }

    static void Bind(int fd, int port)
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            LOG_MESSAGE(ERROR, "bind error\n");
            exit(1);
        }
        LOG_MESSAGE(DEBUG, "Bind fd: %d\n", fd);
    }

    static void Listen(int fd)
    {
        if (listen(fd, backlog) < 0)
        {
            LOG_MESSAGE(ERROR, "listen error\n");
            exit(1);
        }
        LOG_MESSAGE(DEBUG, "Listen fd: %d\n", fd);
    }

    static int Accept(int sock, std::string& ip, uint16_t& port)
    {
        struct  sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int fd = accept(sock, (struct sockaddr*)&addr, &len);
        if(fd < 0)
        {
            std::cerr << "accept error" << std::endl;
            exit(1);
        }
        ip = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        LOG_MESSAGE(DEBUG, "Accept fd: %d\n", fd);
        return fd;
    }
};

#endif // SOCK_HPP