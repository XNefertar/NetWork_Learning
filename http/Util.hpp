#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <iostream>
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

};



#endif // _UTIL_HPP