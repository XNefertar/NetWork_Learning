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

bool callback(const HttpRequest &req, HttpResponse &res)
{
    // cout << "Received: " << req._inbuffer << endl;

    // // UTF-8 转 GBK
    // // 使用Linux系统自带的iconv库
    // string utf8_str = "你好啊！！！";
    // string gbk_str;
    // iconv_t cd = iconv_open("GBK", "UTF-8");
    // if (cd == (iconv_t)-1)
    // {
    //     cerr << "iconv_open failed" << endl;
    //     return false;
    // }

    // size_t inbytesleft = utf8_str.size();
    // size_t outbytesleft = inbytesleft * 2; // GBK 编码可能比 UTF-8 长
    // char* inbuf = const_cast<char*>(utf8_str.c_str());
    // char outbuf[outbytesleft];
    // char* poutbuf = outbuf;

    // if (iconv(cd, &inbuf, &inbytesleft, &poutbuf, &outbytesleft) == (size_t)-1)
    // {
    //     cerr << "iconv failed" << endl;
    //     iconv_close(cd);
    //     return false;
    // }

    // gbk_str.assign(outbuf, poutbuf - outbuf);
    // iconv_close(cd);
    // res._outbuffer = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(gbk_str.size()) + "\r\n\r\n" + gbk_str;

    // cout << "----------------------http start---------------------------" << endl;
    // std::cout << req._inbuffer << std::endl;
    // cout << "----------------------http end---------------------------" << endl;


    std::string respline = "HTTP/1.1 200 OK\r\n";
    std::string respheader = "Content-Type: text/html\r\n";
    std::string respblank = "\r\n";

    std::string body;
#ifdef TEST
    std::string content;
    if (Util::readFile("test.html", &content))
    {
        std::cout << "File content:\n" << content << std::endl;
    }
    else
    {
        std::cerr << "Failed to read file" << std::endl;
    }
#endif  
    if (!Util::readFile(req.getPath(), &body))
    {
        Util::readFile(errUrl, &body);
    }

    res._outbuffer = respline + respheader + respblank + body; // 返回给客户端的数据
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    unique_ptr<ServerHttp> server(new ServerHttp(callback, atoi(argv[1])));
    server->init();
    server->run();
    return 0;
}