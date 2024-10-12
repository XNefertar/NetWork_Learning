#ifndef _TASK_HANDLER_HPP
#define _TASK_HANDLER_HPP

#include <iostream>
#include <string>
#include <functional>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

using task_func_ptr = void(*)(int);

class Task{
private:
    task_func_ptr _fptr;
    int _socketfd;
public:
    // 构造函数
    Task() = default;
    Task(task_func_ptr fptr, int socketfd)
        :_fptr(fptr), 
        _socketfd(socketfd)
    {}

    void operator()(){
        _fptr(_socketfd);
    }

    ~Task() = default;
};

#endif