#ifndef _SELECT_SERVER_HPP
#define _SELECT_SERVER_HPP

#include <iostream>
#include "sock.hpp"

namespace Select
{

    static const int default_port = 8888;
    class SelectServer
    {
    public:
        SelectServer(int port = default_port)
            : _port(port),
              _listen_fd(-1)
        {
        }

        void HandlerEvent(int listen_fd, fd_set &rfds)
        {
            if (FD_ISSET(listen_fd, &rfds))
            {
                std::string client_ip;
                uint16_t client_port;
                int sock = Sock::Accept(listen_fd, client_ip, client_port);
                if (sock < 0)
                    return;
                std::cout << "client ip: " << client_ip << " port: " << client_port << std::endl;
            }
        }

        void init()
        {
            _listen_fd = Sock::Socket();
            Sock::Bind(_listen_fd, _port);
            Sock::Listen(_listen_fd);
        }
        void start()
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(_listen_fd, &rfds);
            struct timeval timeout = {5, 0};

            for (;;)
            {
                // 本质是阻塞式等待
                // Accept = 等 + 获取
                // 1. 等：等待客户端连接
                // 2. 获取：获取客户端连接
                // std::string client_ip;
                // uint16_t client_port;
                // int sock = Sock::Accept(_listen_fd, client_ip, client_port);
                // if(sock < 0) continue;

                // 采用select模型
                int n = select(_listen_fd + 1, &rfds, nullptr, nullptr, nullptr);
                switch (n)
                {
                case -1:
                    std::cerr << "select error" << std::endl;
                    break;
                case 0:
                    std::cout << "select timeout" << std::endl;
                    break;
                default:
                    HandlerEvent(_listen_fd, rfds);
                    break;
                }
            }
        }

        ~SelectServer() {}

    private:
        int _port;
        int _listen_fd;
    };
}

#endif // _SELECT_SERVER_HPP