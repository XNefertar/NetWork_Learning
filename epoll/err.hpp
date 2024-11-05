#ifndef __ERR_HPP__
#define __ERR_HPP__

#include <iostream>

enum
{
    USAGE_ERR = 1,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    EPOLL_CREATE_ERR
};

#endif