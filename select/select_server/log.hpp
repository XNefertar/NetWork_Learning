#ifndef _LOG_HPP_ 
#define _LOG_HPP_

#include <iostream>
#include <string>
#include <cstdarg>
#include <ctime>
#include <unistd.h>

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

const char * to_levelstr(int level)
{
    switch(level)
    {
        case DEBUG : return "DEBUG";
        case NORMAL: return "NORMAL";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case FATAL: return "FATAL";
        default : return nullptr;
    }
}

void logMessage(int level, const char *format, ...)
{
#define NUM 1024
    char logprefix[NUM];
    snprintf(logprefix, sizeof(logprefix), "[%s][%ld][pid: %d]",
        to_levelstr(level), (long int)time(nullptr), getpid());

    char logcontent[NUM];
    va_list arg;
    va_start(arg, format);
    vsnprintf(logcontent, sizeof(logcontent), format, arg);

    std::cout << logprefix << logcontent << std::endl;
}


#endif // _LOG_HPP_