#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <iostream>
#include <fstream>
#include <string>

class Util
{
public:
    static std::string getOnline(std::string inbuffer, std::string sep)
    {
        auto pos = inbuffer.find(sep);
        if (pos == std::string::npos)
        {
            return "";
        }
        auto temp = inbuffer.substr(0, pos);
        inbuffer = inbuffer.substr(pos + sep.size());
        return temp;
    }

    static bool readFile(const std::string &path, std::string* outbuffer)
    {
        std::ifstream in(path);
        if (!in.is_open())
        {
            return false;
        }
        std::string line;
        while(std::getline(in, line))
        {
            *outbuffer += line;
        }
        in.close();
        return true;
    }

};



#endif // _UTIL_HPP