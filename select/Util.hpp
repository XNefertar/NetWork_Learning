#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fcntl.h>

namespace Util
{
    void task1()
    {
        std::cout << "task1" << std::endl;
    }

    void task2()
    {
        std::cout << "task2" << std::endl;
    }

    void task3()
    {
        std::cout << "task3" << std::endl;
    }

    void set_nonblock(int fd)
    {
        int f = fcntl(fd, F_GETFL, 0);
        if(f < 0)
        {
            std::cerr << "fcntl error" << std::endl;
            return;
        }
        fcntl(fd, F_SETFL, f | O_NONBLOCK);
    }
}




#endif // _UTIL_HPP