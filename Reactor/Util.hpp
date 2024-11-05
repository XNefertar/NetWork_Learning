#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <fcntl.h>

class Util
{
public:
    // 设置文件描述符为非阻塞
    // 满足 Reactor 模型
    static bool SetNonBlock(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1)
        {
            logMessage(ERROR, "fcntl F_GETFL error\n");
            return false;
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            logMessage(ERROR, "fcntl F_SETFL error\n");
            return false;
        }
        return true;
    }

};


#endif // __UTIL_HPP__