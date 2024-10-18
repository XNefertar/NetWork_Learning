#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <iostream>
#include <fstream>
#include <string>

class Util
{
public:
    // sep  \r\n
    // 获取请求行
    static std::string getOnline(std::string& inbuffer, const std::string sep)
    {
        auto pos = inbuffer.find(sep);
        if (pos == std::string::npos)
        {
            return "";
        }
        std::string sub = inbuffer.substr(0, pos);
        return sub;
    }

    static bool readFile(const std::string &path, char *outbuffer, int size)
    {
        std::ifstream in(path, std::ios::binary); // 以二进制模式打开文件
        if (!in.is_open())
        {
            return false;
        }

        in.read(outbuffer, size);
        in.close();
        return true;
    }
};

#endif // _UTIL_HPP