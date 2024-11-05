#ifndef __REACTOR_HPP__
#define __REACTOR_HPP__

#include <unordered_map>
#include <functional>
#include <sys/epoll.h>

#include "protocol.hpp"
#include "epoll.hpp"
#include "Sock.hpp"
#include "Util.hpp"
#include "Err.hpp"
#include "Log.hpp"

namespace ReactorServer
{

    struct Connections;
    class Reactor;
    using funcPtr = std::function<void(Connections *)>;
    const static uint64_t default_port = 8080;
    const static int max_events = 1024;
    const static int default_timeout = -1;
    const static int default_value = -1;
    const static int default_backlog = 5;
    const static int default_maxSize = 1024;

    // 对连接进行封装
    struct Connections
    {
        int _sockfd;
        std::string _inbuffer;  // 输入缓冲区
        std::string _outbuffer; // 输出缓冲区

        funcPtr _sendHandler;  // 发送处理函数
        funcPtr _recvHandler;  // 读取处理函数
        funcPtr _excepHandler; // 异常处理函数

        Reactor *_reactor; // 指向 Reactor 对象

        Connections(int sockfd, Reactor *reactor)
            : _sockfd(sockfd),
              _inbuffer{},
              _outbuffer{},
              _sendHandler(nullptr),
              _recvHandler(nullptr),
              _excepHandler(nullptr),
              _reactor(reactor)
        {
        }

        ~Connections()
        {
            close(_sockfd);
        }

        void registerHandler(funcPtr sendHandler, funcPtr recvHandler, funcPtr excepHandler)
        {
            _sendHandler = sendHandler;
            _recvHandler = recvHandler;
            _excepHandler = excepHandler;
        }
    };

    class Reactor
    {
    private:
        int _listen_fd;
        uint64_t _port;
        Epoll _epoll;
        funcPtr _taskHandler; // 任务处理函数
        epoll_event *_events; // struct epoll_event 数组

        std::unordered_map<int, std::shared_ptr<Connections>> _conns; // 建立映射

    private:
        void Loop(int timeout)
        {
            int n = _epoll.WaitEvent(_events, max_events, timeout);
            switch (n)
            {
            case -1:
                logMessage(ERROR, "epoll_wait error\n");
                break;
            case 0:
                logMessage(NORMAL, "epoll_wait timeout\n");
                break;
            default:
                HandlerEvent(n);
                break;
            }
        }

        void HandlerEvent(int n)
        {
            for (int i = 0; i < n; ++i)
            {
                int fd = _events[i].data.fd;
                if (fd == _listen_fd)
                {
                    AcceptHandler(fd);
                }
                else
                {
                    // 处理已有连接
                    ConnectionHandler(_events[i].events, fd);
                }
            }
        }

        void ConnectionHandler(uint32_t events, int fd)
        {
            auto iter = _conns.find(fd);
            if (iter == _conns.end())
            {
                logMessage(ERROR, "can't find fd\n");
                return;
            }

            if (events & EPOLLIN)
            {
                iter->second->_recvHandler(iter->second.get());
            }
            if (events & EPOLLOUT)
            {
                iter->second->_sendHandler(iter->second.get());
            }
            if (events & EPOLLERR)
            {
                iter->second->_excepHandler(iter->second.get());
            }
        }

    private:
        void AcceptHandler(int listenfd)
        {
            std::string ip;
            uint16_t port;
            int conn_sock = Sock::Accept(listenfd, ip, port);
            if (conn_sock == -1)
            {
                logMessage(ERROR, "accept error\n");
                return;
            }

            AddConnection(conn_sock, EPOLLIN | EPOLLET,
                          std::bind(&Reactor::SendHandler, this, std::placeholders::_1),
                          std::bind(&Reactor::RecvHandler, this, std::placeholders::_1),
                          std::bind(&Reactor::ExcepHandler, this, std::placeholders::_1));
        }

        void SendHandler(Connections *conns)
        {
            for (;;)
            {
                ssize_t n = send(conns->_sockfd, conns->_outbuffer.c_str(), conns->_outbuffer.size(), 0);
                if(n > 0)
                {
                    // 发送完毕
                    if(conns->_outbuffer.size() == n)
                    {
                        conns->_outbuffer.clear();
                        _epoll.ControlEvent(conns->_sockfd, EPOLLIN | EPOLLET, EPOLL_CTL_MOD);
                        break;
                    }
                    // 发送部分
                    else
                    {
                        conns->_outbuffer = conns->_outbuffer.substr(n);
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    else if (errno == EINTR)
                        continue;
                    else
                    {
                        if (conns->_excepHandler)
                        {
                            conns->_excepHandler(conns);
                            return;
                        }
                    }
                }
            }
        }

        void RecvHandler(Connections *conns)
        {
            char buffer[default_maxSize];
            for (;;)
            {
                int n = recv(conns->_sockfd, buffer, sizeof(buffer) - 1, 0);
                if (n > 0)
                {
                    buffer[n] = '\0'; // 确保字符串以 '\0' 结尾
                    conns->_inbuffer += buffer;
                    if (_taskHandler)
                    {
                        _taskHandler(conns);
                    }
                    logMessage(DEBUG, "recv: %s\n", conns->_inbuffer.c_str());
                }
                else if (n == 0)
                {
                    if (conns->_excepHandler)
                    {
                        conns->_excepHandler(conns);
                        return;
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    else if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        logMessage(ERROR, "recv error: %s\n", strerror(errno));
                        break;
                    }
                }
            }
        }

        void ExcepHandler(Connections *conns)
        {
        }

    private:
        void AddConnection(int sock, uint32_t events, funcPtr sendHandler, funcPtr recvHandler, funcPtr excepHandler)
        {
            // 设置非阻塞
            if (events & EPOLLET)
            {
                Util::SetNonBlock(sock);
            }

            auto conn(new Connections(sock, this));
            conn->registerHandler(sendHandler, recvHandler, excepHandler);

            _epoll.AddEvent(sock, events);
            _conns.insert(std::pair<int, Connections *>(sock, conn));

            logMessage(DEBUG, "Add connection: %d\n", sock);
        }

    public:
        Reactor(funcPtr taskHandler, uint64_t port = default_port)
            : _port(port),
              _listen_fd(-1),
              _events(nullptr),
              _taskHandler(taskHandler)
        {
            logMessage(NORMAL, "Reactor construct\n");
        }

        ~Reactor()
        {
            logMessage(NORMAL, "Reactor destruct\n");
            if (_listen_fd != default_value)
            {
                close(_listen_fd);
            }
            _epoll.Close();
            if (_events != nullptr)
            {
                delete[] _events;
            }
        }

        // 事件派发器
        void Dispatcher()
        {
            for (;;)
            {
                Loop(default_timeout);
            }
        }

        void ReactoInit()
        {
            _listen_fd = Sock::Socket();
            Sock::Bind(_listen_fd, _port);
            Sock::Listen(_listen_fd);

            if (!_epoll.Create(max_events))
            {
                logMessage(ERROR, "epoll_create error\n");
                exit(1);
            }

            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listen_fd;
            if (_epoll.AddEvent(_listen_fd, EPOLLIN) == false)
            {
                logMessage(ERROR, "epoll_ctl error\n");
                exit(1);
            }

            _events = new struct epoll_event[max_events];
            if (_events == nullptr)
            {
                logMessage(ERROR, "new epoll_event error\n");
                exit(1);
            }
        }

        void ReactorRun()
        {
            Dispatcher();
        }
    };

}

#endif // __REACTOR_HPP__