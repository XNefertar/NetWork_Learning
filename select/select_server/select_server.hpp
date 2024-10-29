#ifndef _SELECT_SERVER_HPP
#define _SELECT_SERVER_HPP

#include <iostream>
#include "sock.hpp"

namespace Select
{

    static const int default_port = 8888;
    class SelectServer
    {
    public:
        SelectServer(int port = default_port)
        : _port(port),
        _listen_fd(-1)
        {}

        void init()
        {
            _listen_fd = Sock::Socket();
            Sock::Bind(_listen_fd, _port);
            Sock::Listen(_listen_fd);
        }
        void start()
        {
            for(;;)
            {
                std::string client_ip;
                uint16_t client_port;
                int sock = Sock::Accept(_listen_fd, client_ip, client_port);
                if(sock < 0) continue;

            }
        }

        ~SelectServer() {}

    private:
        int _port;
        int _listen_fd;
    };
}

#endif // _SELECT_SERVER_HPP