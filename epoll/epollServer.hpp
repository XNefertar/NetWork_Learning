#ifndef _EPOLLSERVER_HPP
#define _EPOLLSERVER_HPP

#include <sys/epoll.h>
#include "Sock.hpp"
#include "log.hpp"
#include "err.hpp"

namespace epollServer
{
    static const uint64_t defaultPort       = 8080;
    static const int      defaultEpollSize  = 1024;
    static const int      defaultValue      = -1;
    static const int      timeout           = -1; // epoll_wait timeout，-1表示阻塞等待

    class EpollServer
    {
    private:
        uint64_t _port;
        struct epoll_event *_events;
        int _listenfd;
        int _epfd;

    public:
        EpollServer(uint64_t port = defaultPort)
            : _port(port),
              _listenfd(defaultValue),
              _epfd(defaultValue)
        {
        }
        ~EpollServer() 
        {
            if(_listenfd != defaultValue)
                close(_listenfd);
            if(_epfd != defaultValue)
                close(_epfd);
            if(_events != nullptr)
                delete[] _events;
        }

        void ServerInit()
        {
            // 套接字绑定
            _listenfd = Sock::Socket();
            Sock::Bind(_listenfd, _port);
            Sock::Listen(_listenfd);
            
            // 创建epoll
            _epfd = epoll_create(1);
            if(_epfd < 0)
            {
                logMessage(ERROR, "epoll_create error");
                exit(EPOLL_CREATE_ERR);
            }

            // 将监听套接字加入epoll
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listenfd;
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _listenfd, &ev);

            // 创建epoll事件数组
            _events = new struct epoll_event[defaultEpollSize];

            logMessage(NORMAL, "Server init success\n");
        }

        void ServerStart()
        {
            
            for(;;)
            {
                int n = epoll_wait(_epfd, _events, defaultEpollSize, timeout);
                switch (n)
                {
                case -1:
                    logMessage(ERROR, "epoll_wait error");
                    break;
                case 0:
                    logMessage(DEBUG, "epoll_wait timeout\n");
                    break;
                default:
                    HandlerEvent(n);
                }

            }
        }
        
        void HandlerEvent(int ReadyNum)
        {
            for(int i = 0; i < ReadyNum; ++i)
            {
                int sockfd = _events[i].data.fd;
                if(sockfd == _listenfd && _events[i].events & EPOLLIN)
                {
                    std::string ip;
                    uint16_t port;
                    int connfd = Sock::Accept(_listenfd, ip, port);
                    logMessage(NORMAL, "Accept connfd: %d, ip: %s, port: %d\n", connfd, ip.c_str(), port);
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = connfd;
                    epoll_ctl(_epfd, EPOLL_CTL_ADD, connfd, &ev);
                }
                else if(_events[i].events & EPOLLIN)
                {
                    char buf[1024] = {0};
                    int n = read(sockfd, buf, sizeof(buf));
                    if(n < 0)
                    {
                        logMessage(ERROR, "read error");
                        close(sockfd);
                        continue;
                    }
                    else if(n == 0)
                    {
                        epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
                        logMessage(NORMAL, "client close\n");
                        close(sockfd);
                        continue;
                    }
                    std::string msg(buf);
                    msg = "server # " + msg;
                    write(sockfd, msg.c_str(), msg.size());
                    buf[n - 1] = '\0';
                    logMessage(NORMAL, "recv msg: %s\n", buf);
                }
            }
        }
    };
}

#endif