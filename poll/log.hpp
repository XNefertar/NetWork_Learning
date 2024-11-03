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

const char* get_color(int level)
{
    switch(level)
    {
        case DEBUG : return "\033[0m";      // 默认颜色
        case NORMAL: return "\033[34m";     // 蓝色
        case WARNING: return "\033[33m";    // 黄色
        case ERROR: return "\033[31m";      // 红色
        case FATAL: return "\033[35m";      // 紫色
        default : return "\033[0m";         // 默认颜色
    }
}

void logMessage(int level, const char *format, ...)
{
    const char* color = get_color(level);
    const char* level_str = to_levelstr(level);
    if (level_str == nullptr) return;

    // 获取当前时间
    time_t now = time(nullptr);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // 打印日志等级和时间
    std::cout << color << "[" << time_str << " " << level_str << "] ";

    // 重置颜色
    std::cout << "\033[0m";

    // 打印日志消息
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

}

#endif // _LOG_HPP_