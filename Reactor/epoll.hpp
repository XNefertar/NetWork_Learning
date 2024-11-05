#ifndef __EPOLL_HPP__
#define __EPOLL_HPP__

#include <unistd.h>
#include <sys/epoll.h>
// 封装系统调用 epoll
class Epoll
{
private:
    int _epoll_fd;

public:
    Epoll()
        : _epoll_fd(-1)
    {
    }
    ~Epoll()
    {
        if (_epoll_fd != -1)
        {
            close(_epoll_fd);
        }
    }

public:
    // 封装 epoll 的增删查改
    bool Create(int size)
    {
        _epoll_fd = epoll_create(size);
        if (_epoll_fd == -1)
        {
            return false;
        }
        return true;
    }

    bool AddEvent(int sock, uint32_t event)
    {
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = sock;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sock, &ev) == -1)
        {
            return false;
        }
        return true;
    }

    int WaitEvent(struct epoll_event *events, int max_events, int timeout)
    {
        int n = epoll_wait(_epoll_fd, events, max_events, timeout);
        return n;
    }

    bool ControlEvent(int sock, uint32_t event, int op)
    {
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = sock;
        if (epoll_ctl(_epoll_fd, op, sock, &ev) == -1)
        {
            return false;
        }
        return true;
    }

    void Close()
    {
        if (_epoll_fd != -1)
        {
            close(_epoll_fd);
        }
    }
};

#endif // __EPOLL_HPP__