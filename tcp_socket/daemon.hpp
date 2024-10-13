#ifndef _DEAMON_HPP
#define _DEAMON_HPP

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void daemon(const char* path = nullptr)
{
    pid_t pid = fork();

    if(pid < 0){
        std::cerr << "fork error" << std::endl;
        exit(1);
    }
    else if(pid > 0) { exit(0); }

    setsid();
    if (path) chdir(path);
    
    
    // 直接关闭标准输入输出（不推荐）
    // for(int i = 0; i < 3; ++i){
    //     close(i);
    // }


    // 重定向标准输入输出、标准错误
    // /dev/null是一个特殊的文件，写入到这个文件的内容会被丢弃，从这个文件读取内容会返回EOF
    int fd = open("/dev/null", O_RDWR);
    if(fd < 0){
        std::cerr << "open /dev/null error" << std::endl;
        exit(1);
    }
    // 重定向标准输入输出、标准错误
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    close(fd);
}


#endif