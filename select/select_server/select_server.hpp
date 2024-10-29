#ifndef _SELECT_SERVER_HPP
#define _SELECT_SERVER_HPP

#include <iostream>
#include <vector>
#include "sock.hpp"

namespace Select
{

    static const int default_port = 8888;
    static const int max_fdset = sizeof(fd_set) * 8;
    static const int default_val = -1;

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

                // 将新的sock加入到fd_array中
                // 找到未使用的位置
                // 将sock加入到fd_array中
                int index = 0;
                for(index = 0; index < max_fdset; ++index)
                {
                    if(fd_array[i] == default_val)
                    {
                        fd_array[i] = sock;
                        break;
                    }
                }
                if(index == max_fdset)
                {
                    std::cerr << "fd_array is full" << std::endl;
                    close(sock);
                }
            }
        }

        void init()
        {
            _listen_fd = Sock::Socket();
            Sock::Bind(_listen_fd, _port);
            Sock::Listen(_listen_fd);
            fd_array = new int[max_fdset]{};
            for (int i = 0; i < max_fdset; ++i)
                fd_array[i] = default_val;
            fd_array[_listen_fd] = _listen_fd;
        }
        
        void start()
        {

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
                // ---------------------------------------------------------------
                // ---------------------------------------------------------------

                fd_set rfds;
                FD_ZERO(&rfds);

                int max_fd = _listen_fd;
                for (int i = 0; i < max_fdset; ++i)
                {
                    if (fd_array[i] == default_val)
                        continue;
                    FD_SET(fd_array[i], &rfds);
                    max_fd = std::max(max_fd, fd_array[i]);
                }
                
                struct timeval timeout = {5, 0};
                // 采用select模型
                int n = select(max_fd + 1, &rfds, nullptr, nullptr, nullptr);
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

        ~SelectServer() 
        {
            if(_listen_fd != -1)
                close(_listen_fd);
            delete[] fd_array;
        }

    private:
        int _port;
        int _listen_fd;
        int *fd_array;
    };
}

#endif // _SELECT_SERVER_HPP