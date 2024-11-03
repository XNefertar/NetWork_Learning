#ifndef _POLL_SERVER_HPP
#define _POLL_SERVER_HPP

#include <iostream>
#include <poll.h>
#include "Sock.hpp"
#include "log.hpp"
#include "error.h"
#include "LogMacros.hpp"

/*
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int fd;         // 文件描述符
    short events;   // 等待的事件
    short revents;  // 实际发生的事件（就绪的事件）
};
*/

namespace Poll
{
    class PollServer
    {
        static const int default_port = 8080;
        static const int max_fdset = 1024;
        static const int default_val = -1;

    public:
        PollServer(int port = default_port)
            : _listen_fd(-1),
              _rfds(nullptr)
        {
        }

        void ListenEvent()
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(_listen_fd, (struct sockaddr *)&client_addr, &len);
            if (client_fd < 0)
            {
                logMessage(ERROR, "accept error");
                return;
            }
            logMessage(NORMAL, "accept a new client: %s \n", inet_ntoa(client_addr.sin_addr));
            // 将client_fd加入到rfds中
            // 记录为读事件
            for (int i = 0; i < max_fdset; ++i)
            {
                if (_rfds[i].fd == default_val)
                {
                    _rfds[i].fd = client_fd;
                    _rfds[i].events = POLLIN;
                    break;
                }
            }
        }

        void RecvEvent()
        {
            for (int i = 1; i < max_fdset; ++i)
            {
                if (_rfds[i].fd == default_val)
                    continue;
                // 处理读事件
                if (_rfds[i].revents & POLLIN)
                {
                    char buf[1024];
                    memset(buf, 0, sizeof(buf));
                    int n = recv(_rfds[i].fd, buf, sizeof(buf), 0);
                    if (n < 0)
                    {
                        logMessage(ERROR, "recv error");
                    }
                    else if (n == 0)
                    {
                        logMessage(NORMAL, "client close");
                        close(_rfds[i].fd);
                        _rfds[i].fd = default_val;
                    }
                    else
                    {
                        logMessage(NORMAL, "recv: %s", buf);
                        buf[n] = '\0';
                        send(_rfds[i].fd, buf, n, 0);
                    }
                }
            }
        }

        void HandlerEvent()
        {
            if (_rfds[0].revents & POLLIN)
            {
                ListenEvent();
            }
            else
            {
                RecvEvent();
            }
        }

        void ServerInit()
        {
            _listen_fd = Sock::Socket();
            Sock::Bind(_listen_fd, default_port);
            Sock::Listen(_listen_fd);
            _rfds = new struct pollfd[max_fdset]{};
            for (int i = 0; i < max_fdset; ++i)
            {
                _rfds[i].fd = default_val;
            }
            // 将监听事件加入到rfds中
            _rfds[0].fd = _listen_fd;
            _rfds[0].events = POLLIN;
        }

        void ServerStart()
        {
            for (;;)
            {
                int n = poll(_rfds, max_fdset, -1);
                if (n < 0)
                {
                    logMessage(ERROR, "poll error");
                    break;
                }
                else if (n == 0)
                {
                    logMessage(WARNING, "poll timeout");
                    continue;
                }
                else
                {
                    HandlerEvent();
                }
            }
        }

    private:
        int _listen_fd;
        // 这里只考虑读事件
        struct pollfd *_rfds;
    };

}

#endif // _POLL_SERVER_HPP