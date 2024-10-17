#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <string>
#include <vector>
#include <sstream>
#include "Util.hpp"

const std::string sep                   =  "\r\n";
const std::string defaultStatus         =  "200 OK";
const std::string defaultVersion        =  "HTTP/1.1";
const std::string defaultContentType    =  "text/html";
const std::string defaultRoot           =  "./wwwroot";
const std::string home_page             =  "index.html";
const std::string defaultContentLength  =  "Content-Length: ";
const std::string defaultConnection     =  "Connection: close";
const std::string errUrl                =  "404.html";
const std::string defaultErr            =  "404 Not Found";

namespace Protocol
{
    
    class HttpRequest
    {
    public:
        HttpRequest() = default;
        ~HttpRequest() = default;
        void parse(const std::string &inbuffer)
        {
            std::string line = Util::getOnline(inbuffer, sep);
            if(line.empty())
            {
                return;
            }
            std::cout << "line # " << line << std::endl;
            std::stringstream ss(line);
            ss >> _method >> _url >> _version;
            _path = defaultRoot + _url;
            if(_path[_path.size() - 1] == '/')
            {
                _path += home_page;
            }

            std::cout << "method # " << _method << std::endl;
            std::cout << "path # " << _path << std::endl;
            std::cout << "version # " << _version << std::endl;

        }

        std::string getMethod()   const { return _method; }
        std::string getUrl()      const { return _url; }
        std::string getVersion()  const { return _version; }
        std::string getPath()     const { return _path; }
        std::string& getInbuffer() { return _inbuffer; }

    private:
        std::string _inbuffer;
        // std::string _reqLine;
        // std::vector<std::string> _reqHandler;
        std::string _method;
        std::string _url;
        std::string _version;
        std::string _path;
    };



    class HttpResponse
    {
    public:
        std::string _outbuffer;

    };
}






#endif