#ifndef _LOG_HPP
#define _LOG_HPP

#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>

#define NORMAL 0
#define WARNING 1
#define ERROR 2
#define FATAL 3
#define DEBUG 4
#define INFO 5

#define LOG_LEVEL int

std::string to_log(LOG_LEVEL level)
{
    switch (level)
    {
    case NORMAL:
        return "NORMAL";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    default:
        return "UNKNOWN";
    }
}

void logMessage(LOG_LEVEL level, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    char buffer[1024]{};
    sprintf(buffer, "[%s][%ld][%ld] ", to_log(level).c_str(), time(nullptr), getpid());


    char response[1024]{};
    vsprintf(response, message, args);

    std::cout << buffer << response << std::endl;


    va_end(args);
}

#endif