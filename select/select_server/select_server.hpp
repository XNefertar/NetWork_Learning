#ifndef _SELECT_SERVER_HPP
#define _SELECT_SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
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

        void Print()
        {
            for (int i = 0; i < max_fdset; ++i)
            {
                if (fd_array[i] != default_val)
                    std::cout << "fd_array[" << i << "] = " << fd_array[i] << std::endl;
            }
        }

        void ListenFunc(int listen_fd)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
            if (client_fd < 0)
            {
                std::cerr << "accept error" << std::endl;
                return;
            }
            std::cout << "accept a new client: " << inet_ntoa(client_addr.sin_addr) << std::endl;
            // 将client_fd加入到fd_array中
            for (int j = 0; j < max_fdset; ++j)
            {
                if (fd_array[j] == default_val)
                {
                    fd_array[j] = client_fd;
                    break;
                }
            }
        }

        void RecvFunc(int fd)
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            int n = recv(fd, buf, sizeof(buf), 0);
            if (n <= 0)
            {
                std::cerr << "recv error" << std::endl;
                close(fd);
                for (int i = 0; i < max_fdset; ++i)
                {
                    if (fd_array[i] == fd)
                    {
                        fd_array[i] = default_val;
                        break;
                    }
                }
            }
            else
            {
                // 删除buf中的换行符
                buf[n - 1] = '\0';
                std::cout << "recv: " << buf << std::endl;
                std::string str = "server # " + std::string(buf) + "\n";
                send(fd, str.c_str(), str.size(), 0);
            }
        }

        void HandlerEvent(int listen_fd, fd_set &rfds)
        {
            // 过滤非法的fd
            for (int i = 0; i < max_fdset; ++i)
            {
                if (fd_array[i] == default_val)
                    continue;
                if (FD_ISSET(fd_array[i], &rfds))
                {
                    if (fd_array[i] == listen_fd)
                    {
                        ListenFunc(listen_fd);
                    }
                    else
                    {
                        RecvFunc(fd_array[i]);
                    }
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
            fd_array[0] = _listen_fd;
        }

        void start()
        {
            for (;;)
            {
                fd_set rfds;
                FD_ZERO(&rfds);

                int max_fd = _listen_fd;
                // 将fd_array中的有效fd加入到rfds中
                // 更新max_fd
                for (int i = 0; i < max_fdset; ++i)
                {
                    if (fd_array[i] == default_val)
                        continue;
                    FD_SET(fd_array[i], &rfds);
                    max_fd = std::max(max_fd, fd_array[i]);
                }

                // struct timeval timeout = {5, 0};
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
            if (_listen_fd != -1)
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