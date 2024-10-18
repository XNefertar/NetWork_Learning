#include "server_http.hpp"
#include "protocol.hpp"
#include <memory>
#include <iconv.h>

using namespace std;
using namespace server;

static void Usage(string proc)
{
    cerr << "\nUsage:\n\t" << proc << " server_port\n\n";
}

string suffixToDesc(const string &suffix)
{
    std::string ct = "Content-Type: ";
    if (suffix == ".html")
        ct += "text/html";
    else if (suffix == ".jpg")
        ct += "application/x-jpg";
    ct += "\r\n";
    return ct;
}

bool Get(const HttpRequest &req, HttpResponse &res)
{
    std::cout << "----------------------http start---------------------------" << std::endl;
    std::cout << req.getInbuffer() << std::endl;
    std::cout << "method: " << req.getMethod() << std::endl;
    std::cout << "url: " << req.getUrl() << std::endl;
    std::cout << "httpversion: " << req.getVersion() << std::endl;
    std::cout << "path: " << req.getPath() << std::endl;
    std::cout << "suffix: " << req.getSuffix() << std::endl;
    std::cout << "size: " << req.getSize() << "字节" << std::endl;
    std::cout << "----------------------http end---------------------------" << std::endl;

    std::string respline = "HTTP/1.1 200 OK\r\n";
    std::string respheader = suffixToDesc(req.getSuffix());
    if (req.getSize() > 0)
    {
        respheader += "Content-Length: " + std::to_string(req.getSize()) + "\r\n";
    }

    std::string respblank = "\r\n";
    std::string body;
    body.resize(req.getSize() + 1);

    if (!Util::readFile(req.getPath(), &body[0], req.getSize()))
    {
        Util::readFile(errUrl, &body[0], req.getSize());
    }

    res._outbuffer = respline + respheader + respblank + body;
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    unique_ptr<ServerHttp> server(new ServerHttp(Get, atoi(argv[1])));
    server->init();
    server->run();
    return 0;
}