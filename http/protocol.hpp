#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include "Util.hpp"


const std::string sep = "\r\n";
const std::string home_page = "index.html";
const std::string defaultRoot = "./wwwroot";
const std::string errUrl = "wwwroot/404.html";

// const std::string defaultStatus = "200 OK";
// const std::string defaultVersion = "HTTP/1.1";
// const std::string defaultContentType = "text/html";
// const std::string defaultContentLength = "Content-Length: ";
// const std::string defaultConnection = "Connection: close";
// const std::string defaultErr = "404 Not Found";

namespace Protocol
{

    class HttpRequest
    {
    public:
        HttpRequest() = default;
        ~HttpRequest() = default;

        // 分割请求行
        void parse()
        {
            std::string line = Util::getOnline(_inbuffer, sep);
            if (line.empty())
            {
                return;
            }
            
            std::stringstream ss(line);
            ss >> _method >> _url >> _version;
            
            _path = defaultRoot + _url;
            if (_path[_path.size() - 1] == '/')
            {
                _path += home_page;
            }

            // 4. 获取path对应的资源后缀
            auto pos = _path.rfind(".");
            if (pos == std::string::npos)
                _suffix = ".html";
            else
                _suffix = _path.substr(pos);

            // 5. 得到资源的大小
            struct stat st;
            int n = stat(_path.c_str(), &st); // 使用 _path 而不是 path
            if (n == 0)
                _size = st.st_size;
            else
                _size = -1;
        }

        std::string getMethod() const { return _method; }
        std::string getUrl() const { return _url; }
        std::string getVersion() const { return _version; }
        std::string getPath() const { return _path; }
        std::string getInbuffer() const { return _inbuffer; }
        std::string getSuffix() const { return _suffix; }

        int getSize() const { return _size; }
        void setInbuffer(const std::string &inbuffer) { _inbuffer = inbuffer; }

    private:
        std::string _inbuffer;
        std::string _method;
        std::string _url;
        std::string _version;
        std::string _path;
        std::string _suffix;
        int _size;
    };

    class HttpResponse
    {
    public:
        std::string _outbuffer;
    };
}

#endif